if (navigator.userAgent.toLowerCase().match(/android/i) == "android" || navigator.userAgent.toLowerCase().match(/MicroMessenger/i) == "micromessenger") {
    $("#music-warp").css({"width": "300px", "height": "300px"});
    $("#cover").css({"width": "200px", "height": "200px", "top": "50px", "left": "50px"});
} else if (navigator.userAgent.toLowerCase().match(/iphone os/i) == "iphone os") {
    alert("不对iphone做适配，抱歉！");
}

var music = new Array();
var music_name = new Array();
var music_cover = new Array();
var music_lyric = new Array();
$.getJSON("https://api.i-meto.com/meting/api?server=netease&type=playlist&id=573623248", "", function (data) {//each循环 使用$.each方法遍历返回的数据date
    $.each(data, function (i, item) {
        music[i] = item.url;
        music_name[i]=item.title+"--"+item.author;
        music_cover[i]=item.pic;
        music_lyric[i]=item.lrc;
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
    // $("#music-name").html(music_name[i]);

}

function audioPuase() {
    $("#music-audio").get(0).pause();
    $("#music-warp").css("animation-play-state", "paused");
    $("#player-needle").css("animation-name", "music-needle-rot-another");
    $("#btn-medium-another").hide();
    $("#btn-medium").show();
}
/*lyric begin*/

parseLyric($.ajax({url:music_lyric[0],async:false}).responseText);

function parseLyric(text) {
    var lyric = text.split('\r\n'); //先按行分割
    var _l = lyric.length; //获取歌词行数
    var lrc = new Array(); //新建一个数组存放最后结果
    for (i = 0; i < _l; i++) {
        var d = lyric[i].match(/\[\d{2}:\d{2}((\.|\:)\d{2})\]/g);  //正则匹配播放时间
        var t = lyric[i].split(d); //以时间为分割点分割每行歌词，数组最后一个为歌词正文
        if (d != null) { //过滤掉空行等非歌词正文部分
            //换算时间，保留两位小数
            var dt = String(d).split(':'); //不知道为什么一定要转换时间为字符串之后才能split，难道之前正则获取的时间已经不是字符串了么？
            var _t = Math.round(parseInt(dt[0].split('[')[1]) * 60 + parseFloat(dt[1].split(']')[0]) * 100) / 100; //这一步我自己都觉得甚是坑爹啊！
            lrc.push([_t, t[1]]);
        }
        // return lrc;
    }

    $("#music-audio")[0].ontimeupdate = function () {
        if (this.currentTime > lrc[1][0]) {
            console.log(lrc[1][1]);
            lrc.shift();
        }
    };

/*lyric end*/
$("#btn-medium").click(function () {
    if(i==0)
        resLoad(i);
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
