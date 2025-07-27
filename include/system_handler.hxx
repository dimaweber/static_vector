#pragma once
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <filesystem>
#include <mutex>
#include <cstring>
#if EXPLAIN_AVAILABLE
    #include "libexplain/open.h"
    #include "libexplain/socket.h"
#endif

namespace wbr::sys {
    struct LockGuard {
        explicit LockGuard(pthread_mutex_t *mutex) : _mutex(mutex) { pthread_mutex_lock(_mutex); }

        explicit LockGuard(std::mutex *mutex) : _std_mutex(mutex) { mutex->lock(); }

        ~LockGuard() {
            if (_mutex)
                pthread_mutex_unlock(_mutex);
            if (_std_mutex)
                _std_mutex->unlock();
        }

        LockGuard() = delete;

        LockGuard(const LockGuard &) = delete;

        LockGuard(LockGuard &&) = delete;

        LockGuard &operator=(const LockGuard &) = delete;

        LockGuard &operator=(LockGuard &&) = delete;

    private:
        pthread_mutex_t *_mutex{nullptr};
        std::mutex *_std_mutex{nullptr};
    };

    struct FileDescriptorWrapper {
        FileDescriptorWrapper() = default;

        FileDescriptorWrapper(const char *filename, int opts, mode_t mode) : filePath{filename}, opts{opts}, mode{mode} {
        }

        FileDescriptorWrapper(const char *filename, int opts) : FileDescriptorWrapper(filename, opts, 0) {
        }

        FileDescriptorWrapper(const std::filesystem::path &filename, int opts, mode_t mode = 0) : FileDescriptorWrapper(filename.c_str(), opts, mode) {
        }

        ~FileDescriptorWrapper() { close(); }

        FileDescriptorWrapper(FileDescriptorWrapper &&other) noexcept { swap(std::move(other)); }

        FileDescriptorWrapper &operator=(FileDescriptorWrapper &&other) noexcept {
            if (&other != this)
                swap(std::move(other));
            return *this;
        }

        FileDescriptorWrapper(const FileDescriptorWrapper &o) : filePath{o.filePath}, opts{o.opts}, mode{o.mode}, fd{o.fd > 0 ? ::dup(o.fd) : -1} {
        }

        FileDescriptorWrapper &operator=(const FileDescriptorWrapper &o) {
            if (&o != this) {
                fd = o.fd > 0 ? ::dup(o.fd) : -1;
                filePath = o.filePath;
                mode = o.mode;
                opts = o.opts;
            }
            return *this;
        }

        bool is_open() const { return open() >= 0; }

        operator int() const { return open(); }

        const std::filesystem::path &filepath() const { return filePath; }

        void swap(FileDescriptorWrapper &&o) noexcept {
            std::swap(o.fd, fd);
            std::swap(o.filePath, filePath);
            std::swap(o.opts, opts);
            std::swap(o.mode, mode);
        }

        [[nodiscard]] const std::string &failMsg() const noexcept { return failReason; }

        void setAutoRemove(bool removeOnClose = true) { autoRemove = removeOnClose; }

    protected:
        void close() {
            if (fd > -1) {
                ::close(fd);
            }
            fd = -1;
            if (autoRemove) {
                std::error_code err;
                std::filesystem::remove(filePath, err);
            }
        }

        int open() const {
            if (fd > -1)
                return fd;
            do {
                fd = ::open(filePath.c_str(), opts, mode);
            } while (fd < 0 && errno == EINTR);
            if (fd < 0) {
                std::array<char, 2048> buf;
#if EXPLAIN_AVAILABLE
                const char* msg = buf.data( );
                explain_message_open(buf.data( ), buf.size( ), filePath.c_str( ), opts, mode);
#else
                const char *msg = ::strerror_r(errno, buf.data(), buf.size());
#endif
                failReason.assign(msg);
            } else
                failReason.clear();
            return fd;
        }

        mutable std::string failReason;
        std::filesystem::path filePath;
        int opts;
        mode_t mode;
        mutable int fd{-1};
        bool autoRemove{false};
    };

    inline int close(FileDescriptorWrapper &) = delete;

    struct socket_t {
        socket_t() = default;

        socket_t(int domain, int type, int protocol) {
            fd = ::socket(domain, type, protocol);
            if (!is_open()) {
                std::array<char, 2048> buf;
#if EXPLAIN_AVAILABLE
                const char* msg = buf.data( );
                explain_message_socket(buf.data( ), buf.size( ), domain, type, protocol);
#else
                const char *msg = ::strerror_r(errno, buf.data(), buf.size());
#endif
                failReason.assign(msg);
            }
        }

        explicit socket_t(int sock) : fd{sock} {
        };

        socket_t(socket_t &&s) noexcept {
            std::swap(s.fd, fd);
            std::swap(s.failReason, failReason);
        };

        socket_t &operator=(socket_t &&s) noexcept {
            if (&s != this) {
                std::swap(s.fd, fd);
                std::swap(s.failReason, failReason);
            }
            return *this;
        }

        socket_t(const socket_t &) = delete;

        socket_t &operator=(const socket_t &) = delete;

        ~socket_t() {
            if (fd >= 0)
                ::close(fd);
            fd = -1;
        }

        operator int() const { return fd; }

        bool is_open() const { return fd >= 0; }

        [[nodiscard]] const std::string &failMsg() const noexcept { return failReason; }

    private:
        int fd{-1};
        std::string failReason;
    };

    inline int close(socket_t &) = delete;

    struct SharedMemoryDescriptorWrapper : public FileDescriptorWrapper {
        SharedMemoryDescriptorWrapper(const char *regionName, int opts, mode_t access) { fd = ::shm_open(regionName, opts, access); }
    };

    struct FileWrapper {
        FileWrapper(const std::filesystem::path &filepath, const char *access) : filePath(filepath), pFile(::fopen(filepath.c_str(), access)) {
        }

        ~FileWrapper() { close(); }

        bool is_open() const { return pFile != nullptr; }

        const std::filesystem::path &filepath() const { return filePath; }

        operator FILE *() const { return pFile; }

        FileWrapper() = delete;

        FileWrapper(const FileWrapper &) = delete;

        FileWrapper(FileWrapper &&) = delete;

        FileWrapper &operator=(const FileWrapper &) = delete;

        FileWrapper &operator=(FileWrapper &&) = delete;

        void setAutoRemove(bool removeOnClose = true) { autoRemove = removeOnClose; }

    private:
        std::filesystem::path filePath;
        FILE *pFile{nullptr};
        bool autoRemove{false};

        void close() {
            if (pFile) {
                ::fclose(pFile);
                pFile = nullptr;
            }
            if (autoRemove) {
                std::error_code err;
                std::filesystem::remove(filepath(), err);
            }
        }
    };

    inline int fclose(const FileWrapper &) = delete;

    struct PthreadMutexAttrWrapper {
        PthreadMutexAttrWrapper() { pthread_mutexattr_init(&mutexAttr); }

        ~PthreadMutexAttrWrapper() { pthread_mutexattr_destroy(&mutexAttr); }

        operator pthread_mutexattr_t &() { return mutexAttr; }

        operator pthread_mutexattr_t() const { return mutexAttr; }

        pthread_mutexattr_t *operator&() { return &mutexAttr; }

        const pthread_mutexattr_t *operator&() const { return &mutexAttr; }

    private:
        pthread_mutexattr_t mutexAttr;
    };
}
