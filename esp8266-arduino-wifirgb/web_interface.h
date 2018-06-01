const char WEBINTERFACE[] PROGMEM = R"=====(
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">

<head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <title>WifiRGB Controller Interface</title>
    <script type="text/javascript" src="iro.min.js"></script>
    <script type="text/javascript" src="https://code.jquery.com/jquery-3.3.1.min.js"></script>
    <style>
      body {
        color: #fff;
        background: #272727;
        text-align: center;
      }

      #color-picker-container svg {
        margin: 48px auto;
      }
    </style>
</head>
<body>
  <div id="color-picker-container"></div>
  <script type="text/javascript">
      var size = 0;
      if (window.innerWidth > window.innerHeight) {
        size = window.innerHeight * 0.8;
      } else {
        size = window.innerWidth * 0.8;
      }

      var demoColorPicker = new iro.ColorPicker("#color-picker-container", {
      // color picker options
      // Option guide: https://rakujira.jp/projects/iro/docs/guide.html#Color-Picker-Options
      width: size,
      height: size,
      color: {r: 0, g: 0, b: 0},
      anticlockwise: true,
      borderWidth: 10,
      borderColor: "#313131"
    });

    demoColorPicker.on("color:change", function(color, changes) {
      // Log the color's hex RGB value to the dev console
      console.log(color.rgb);
      console.log(color.hsv.v);
      // If the "H" channel has changed, log the color's HSV value too
      //if (changes.h) {
      //  console.log(color.hsv);
      //}
      var json = {state:"ON",brightness:color.hsv.v,color:{mode:"rgb",r:color.rgb.r,g:color.rgb.g,b:color.rgb.b},mode:"SOLID"};
      console.log(json);
      console.log(JSON.stringify(json));

      $.ajax( "/api/v1/state", { data: JSON.stringify(json), dataType: "json", method: "POST", contentType: "application/json", cache: false, timeout: 2000})
        .done(function( data ) {
          console.log( "Response: " );
          console.log( data );
        })
        .fail(function( data ) {
          console.log( "Error: " );
          console.log( data );
        });
    });
  </script>
</body>
</html>
)=====";
