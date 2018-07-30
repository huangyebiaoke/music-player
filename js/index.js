if (navigator.userAgent.toLowerCase().match(/android/i) == "android" || navigator.userAgent.toLowerCase().match(/MicroMessenger/i) == "micromessenger") {
    $("#music-warp").css({"width": "300px", "height": "300px"});
    $("#cover").css({"width": "200px", "height": "200px", "top": "50px", "left": "50px"});
} else if (navigator.userAgent.toLowerCase().match(/iphone os/i) == "iphone os") {
    alert("不对iphone做适配，抱歉！");
}

var music = new Array();
var music_name = new Array();
var music_cover = new Array();
$.getJSON("https://api.i-meto.com/meting/api?server=netease&type=playlist&id=573623248", "", function (data) {//each循环 使用$.each方法遍历返回的数据date
    $.each(data, function (i, item) {
        music[i] = item.url;
        music_name[i]=item.name+"--"+item.artist;
        music_cover[i]=item.cover;
    });
});

function resLoad(e) {
    // $("#music-name").html(music_name[e]);
    $("#cover").attr("src", music_cover[e]);
    $("#cover-bg").css("background-image", "url(" + music_cover[e] + ")");
    $("#music-audio").attr("src", music[e]);
}

function audioPlay() {
    $("#music-audio").get(0).play();
    $("#music-warp").css("animation-play-state", "running");
    $("#player-needle").css("animation-name", "music-needle-rot");
    $("#btn-medium").hide();
    $("#btn-medium-another").show();
    $("#music-name").html(music_name[i]);
}

function audioPuase() {
    $("#music-audio").get(0).pause();
    $("#music-warp").css("animation-play-state", "paused");
    $("#player-needle").css("animation-name", "music-needle-rot-another");
    $("#btn-medium-another").hide();
    $("#btn-medium").show();
}

$("#btn-medium").click(function () {
    audioPlay();
});

$("#btn-medium-another").click(function () {
    audioPuase();
});
var i = 0;
$("#btn-left").click(function () {
    audioPuase();
    i--;
    if (i < 0) {
        alert("sorry! there's no music.");
        i++;
    }
    resLoad(i);
    audioPlay();
});
$("#btn-right").click(function () {
    audioPuase();
    i++;
    if (i >= music.length) {
        alert("sorry! there's no music.");
        i--;
    }
    resLoad(i);
    audioPlay();
});
$("#music-audio").get(0).addEventListener("timeupdate", function () {
    var scale = this.currentTime / this.duration;
    var allWidth = $("#progress").width() - $("#point").width();
    var leftWidth = allWidth * scale;
    $("#point").css("transform", "translate(" + leftWidth + "px,-3px)");
    $("#point-left").css("width", +leftWidth + "px");
});
$("#point").get(0).addEventListener("touchstart", function (e) {
    var x = e.changedTouches[0].pageX - $(this).offset().left;
    document.addEventListener("touchmove", function (e) {
        var l = e.changedTouches[0].pageX;
        var _left = l - x - $("#progress").offset().left;
        if (_left < 0) {
            _left = 0;
        } else if (_left > $("#progress").width() - $("#point").width()) {
            _left = $("#progress").width() - $("#point").width();
        }
        $("#point").css("transform", "translate(" + _left + "px,-3px)");
        $("#point-left").css("width", +_left + "px");
        var scale = _left / ($("#progress").width() - $("#point").width());
        $("#music-audio").get(0).currentTime = scale * $("#music-audio").get(0).duration;
        return false;
    });
    return false;
});
