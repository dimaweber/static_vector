#pragma once
#include <string_view>
#include <vector>

#include "static_vector.hxx"
#include "string_manipulations.hxx"
#include "string_manipulations_ext.hxx"

namespace wbr::lorem {
class generator_t {
public:
  virtual ~generator_t( )          = default;
  virtual std::string_view next( ) = 0;
};

class loremipsum_t : public generator_t {
private:
  static constexpr size_t                          dictionary_size = 1024;
  static_vector<std::string_view, dictionary_size> words_;
  size_t                                           current_index_ {0};

  void loadDictionary ( ) {
    const std::string lorem_ipsum_text = R"(
    Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum facilisis neque tortor,
    sed ultrices nunc mattis sed. Nulla consectetur sed leo sit amet aliquet.
    Quisque posuere ante sapien, sollicitudin tristique nisl pharetra sed.
    Sed volutpat pretium quam ac facilisis. Fusce ultricies risus id lectus rutrum sagittis.
    Phasellus elementum, lacus in malesuada accumsan, dolor lorem maximus ligula,
    ac ornare tortor diam id sem. Ut dictum felis leo, ac aliquam felis porta at.
    Phasellus egestas orci a sem cursus tincidunt. Praesent non fringilla ipsum.
    Phasellus eu turpis sit amet ante commodo malesuada at eu tellus.
    Nam at risus ac est euismod hendrerit. Vivamus vulputate faucibus ante,
    eget rhoncus dui molestie quis. Integer pellentesque lacus a nisl commodo,
    ut imperdiet nibh sagittis. Duis ultricies arcu vel augue sagittis, id dictum nisl laoreet.
    Vestibulum vel metus eu sem ultrices aliquet.

    Proin hendrerit tellus dolor, eget lobortis metus lacinia nec.
    In ullamcorper accumsan ex eu semper. Nunc vel neque volutpat,
    venenatis ex ac, tincidunt purus. Donec eros ligula, rutrum ac vulputate aliquet,
    rutrum vel orci. Aliquam pulvinar luctus vehicula. Nulla nec egestas lacus.
    In hac habitasse platea dictumst. Aliquam erat volutpat. Donec egestas, ligula sed aliquet ornare,
    turpis est interdum tortor, non viverra odio dolor in odio. Sed urna nisl,
    ornare sed vulputate id, cursus ac elit. Nam dictum ex nec tortor finibus,
    eget scelerisque orci interdum.

    Morbi varius lectus eu orci commodo porta. Cras lacinia metus diam,
    in pharetra magna ullamcorper quis. Aenean sit amet odio id massa feugiat porta.
    In posuere tempus mauris quis pharetra. Nulla sagittis metus ut tempor imperdiet.
    Nullam sit amet venenatis magna, nec maximus metus. Aenean cursus odio vitae neque consequat,
    quis condimentum tortor interdum. Ut gravida nisl suscipit accumsan pharetra.
    Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae;

    Nam dapibus nisi nec urna condimentum vulputate. Phasellus vel ultrices mauris.
    Interdum et malesuada fames ac ante ipsum primis in faucibus. Proin eget purus diam.
    Suspendisse commodo, lacus quis imperdiet ornare, nulla purus fermentum nibh,
    et lobortis nunc tortor vitae ex. Nullam vitae semper nunc. Morbi convallis suscipit nisi,
    at vehicula libero suscipit eu. Praesent vel ultrices nisi. Curabitur vel nibh et risus ullamcorper malesuada ut sed libero.
    Proin ac metus ut magna posuere sollicitudin quis a nisl. Sed tempor ex ut varius ullamcorper.
    Maecenas dictum massa at ex euismod, mattis laoreet arcu tempus. Cras porta faucibus leo,
    gravida finibus leo aliquet ut. Phasellus elementum mi sed ante convallis,
    non fermentum dui varius. Nullam bibendum faucibus tincidunt.

    Sed iaculis aliquam nunc, non tristique libero. Curabitur pretium malesuada leo,
    at ullamcorper erat porta vel. Curabitur tincidunt hendrerit nulla, vel pulvinar dolor gravida vel.
    Suspendisse potenti. Aenean consequat nisi eu enim imperdiet egestas. Vestibulum ultricies et arcu ut auctor.
    Vivamus vitae magna lobortis orci faucibus volutpat sit amet eget quam. Fusce sit amet libero a ante lacinia posuere vel quis dolor.
    Duis pellentesque libero at nisi imperdiet, et pulvinar enim cursus.

    Morbi non tristique sem, sed mattis velit. Vivamus eu dui ut massa rhoncus porta id vitae leo.
    Sed posuere, lectus elementum scelerisque imperdiet, eros lorem imperdiet nibh, sit amet condimentum elit lacus id diam.
    In hac habitasse platea dictumst. Maecenas sit amet mi arcu. Curabitur purus urna,
    faucibus vitae commodo vitae, tincidunt id urna. Phasellus tristique turpis ante, id feugiat urna sagittis nec.
    Curabitur rutrum aliquet tortor blandit pretium. Curabitur molestie nisi ut ex consectetur,
    eu molestie sem pretium. Curabitur sit amet augue gravida, ultricies justo ac, ultricies orci.
    Suspendisse potenti. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus.
    In eget nibh vel magna luctus tempus. Ut cursus augue a vestibulum sollicitudin.
    Sed risus est, volutpat vitae ante vel, tincidunt laoreet velit.

    Nullam mollis at magna sed blandit. Ut aliquet laoreet placerat. Donec scelerisque lacus eu purus suscipit tristique.
    Ut vehicula eros ac tortor fringilla eleifend eget vitae diam. Vivamus hendrerit sem eget fringilla rhoncus.
    Donec bibendum libero nibh, eu vehicula eros pulvinar ac. Vivamus risus libero,
    viverra et convallis non, ultrices sed tellus. Morbi arcu ipsum, porttitor vitae vulputate in, consectetur ac purus.
    Aliquam id dignissim lectus. Suspendisse eget justo hendrerit, ultricies lorem in, feugiat tortor.
    In finibus augue ut tincidunt ultrices. Praesent euismod turpis sit amet enim volutpat pellentesque.
    Aliquam pretium lacus nisi, eu rhoncus elit volutpat at. Ut urna lectus,
    dignissim vel arcu vitae, pretium lobortis arcu. Suspendisse non justo ut velit aliquet malesuada.

    Duis eget convallis erat. Praesent nec ullamcorper lorem. Phasellus non commodo dolor.
    Ut ut magna mattis, luctus neque non, volutpat nulla. Nunc eget risus quam.
    Curabitur pulvinar condimentum neque, quis vestibulum mi consectetur id.
    Vestibulum in orci eget est convallis cursus ac ac odio. Ut magna purus,
    malesuada vitae suscipit quis, sodales non odio. Phasellus feugiat tortor sem, id semper tellus tincidunt tristique.

    Nam sit amet aliquam velit, eu dictum elit. Class aptent taciti sociosqu ad litora torquent per conubia nostra,
    per inceptos himenaeos. Proin lacinia, purus sed facilisis accumsan, lectus diam ultrices risus,
    sed molestie leo elit eget nunc. Curabitur lobortis purus a egestas semper.
    Donec aliquam velit sapien, nec lacinia odio dignissim eu. Curabitur consectetur faucibus arcu id faucibus.
    Nullam molestie ornare quam sed condimentum. Integer vehicula eros egestas mi vestibulum faucibus.
    Cras a libero quis augue sodales rutrum ut quis magna. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
    Fusce posuere turpis nec elit pulvinar, sed pellentesque urna mattis. Curabitur aliquet nulla non ipsum vulputate,
    ac pretium ipsum eleifend. Donec semper, mi vitae pretium elementum, urna nunc ultrices erat, quis consequat urna est non orci.

    In hac habitasse platea dictumst. Maecenas eleifend malesuada justo nec ultrices.
    Quisque gravida molestie risus quis suscipit. Mauris sollicitudin eros vel pellentesque egestas.
    Nulla fermentum nunc eget eros dignissim, tincidunt suscipit risus suscipit. Maecenas quis feugiat ligula,
    et egestas urna. Ut in tristique diam, ac porttitor risus. Nulla aliquet pellentesque mattis.
    Ut commodo venenatis felis, at iaculis risus mollis eu.

    Nullam eget lorem sollicitudin, pharetra dui a, ullamcorper sem. Mauris fringilla elit eget sem aliquet porttitor.
    Aliquam vitae eleifend orci. Nulla vitae convallis nulla, in mattis ligula. Orci varius natoque penatibus et magnis dis parturient montes,
    nascetur ridiculus mus. Etiam sodales imperdiet lacus, et rutrum tellus dignissim sit amet. Maecenas magna.
  )";
    wbr::str::tokenize(lorem_ipsum_text, wbr::str::not_empty, [this] (const std::string_view sv) { words_.push_back(wbr::str::trim(sv, wbr::str::is_punct)); }, " \n\r\t");
  }

public:
  loremipsum_t ( ) {
    loadDictionary( );
  }

  std::string_view next ( ) override {
    if ( current_index_ >= words_.size( ) ) {
      current_index_ = 0;  // Reset to start if we reach the end
    }
    return words_[current_index_++];
  }
};

class composer_t {
public:
  composer_t (generator_t& generator) : generator_(generator) {
  }

  // Return n first words as std::vector<std::string_view>
  std::vector<std::string_view> getTokens (int n) {
    std::vector<std::string_view> tokens;
    for ( int i = 0; i < n; ++i ) {
      tokens.push_back(generator_.next( ));
    }
    return tokens;
  }

  // Return n first words as a string with ' ' between words
  std::string getString (int n, std::string_view separator = " ") {
    const auto tokens = getTokens(n);
    return wbr::str::join(tokens, separator);
  }

  // Return n words as multiline text with max line width w
  std::string getMultilineText (int n, int w) {
    const auto str = getString(n);
    return wbr::str::join(wbr::str::wrap(str, w), "\n");
  }

private:
  generator_t& generator_;
};

}  // namespace wbr::lorem
