<?php
/**
 * Email: linzongho@gmail.com
 * Github: https://github.com/linzongho/Pindex
 * User: asus
 * Date: 8/25/16
 * Time: 11:46 AM
 */

/**
 * Class DevKits
 * @package Lib
 */

function vdebug(){
    static $_messages = [];
    if(func_num_args()){
        return $_messages[] = _vbuildMessage(func_get_args(),debug_backtrace());
    }else{
        return $_messages;
    }
}
/**
 * @param ...
 */
function vdump(){
    echo _vbuildMessage(func_get_args(),debug_backtrace());
}

function vdumpout(){
    exit(_vbuildMessage(func_get_args(),debug_backtrace()));
}

function _vbuildMessage($params,$traces){
    $color='#';$str='9ABCDEF';//随机浅色背景
    for($i=0;$i<6;$i++) $color=$color.$str[rand(0,strlen($str)-1)];
    $str = "<pre style='background: {$color};width: 100%;padding: 10px;margin: 0'><h3 style='color: midnightblue'><b>F:</b>{$traces[0]['file']} << <b>L:</b>{$traces[0]['line']} >> </h3>";
    foreach ($params as $key=>$val) $str .= '<b>Parameter-'.$key.':</b><br />'.print_r($val, true).'<br />';
    return $str.'</pre>';
}

/**
 * @param array $attach 附加变量
 * @param array $keywords 待高亮关键字
 * @author:linzh<784855684@qq.com>
 */
function showtrace(array $attach=[],array $keywords=[]){
    //包含的文件数组
    $files  =  get_included_files();
    $info   =  [];
    if($keywords){
        $replacement = array_values($keywords);
        foreach ($replacement as &$keyword) $keyword = "<b style='background-color: yellow'>{$keyword}</b>";
    }
    foreach ($files as $key=>$file){
        $sise = number_format(filesize($file)/1024,2);
        if(!empty($replacement) ){
            $file = str_replace($keywords,$replacement,$file);
        }
        $info[] = "$file ( $sise KB )";
    }

    //运行时间与内存开销
    $fkey = null;
    $trace = [
        'General'       => [
            'Request'   => date('Y-m-d H:i:s',$_SERVER['REQUEST_TIME']).' '.$_SERVER['SERVER_PROTOCOL'].' '.$_SERVER['REQUEST_METHOD'],
            'SessionID' => session_id(),
            'Cookie'    => var_export($_COOKIE,true),
            'time'      => 1000*(microtime(true) - $_SERVER['REQUEST_TIME_FLOAT']).' ms',
            'Obcache-Size'  => number_format((ob_get_length()/1024),2).' KB (Unexpect Trace Page!)',//不包括trace
        ],
        'Debug'          => vdebug(),
        'Files'         => array_merge(['Total'=>count($info)],$info),
        'GET'           => $_GET,
        'POST'          => $_POST,
        'SERVER'        => $_SERVER,
        'FILES'         => $_FILES,
        'ENV'           => $_ENV,
        'SESSION'       => isset($_SESSION)?$_SESSION:['SESSION state disabled'],//session_start()之后$_SESSION数组才会被创建
        'IP'            => [
            '$_SERVER["HTTP_X_FORWARDED_FOR"]'  =>  isset($_SERVER['HTTP_X_FORWARDED_FOR'])?$_SERVER['HTTP_X_FORWARDED_FOR']:'NULL',
            '$_SERVER["HTTP_CLIENT_IP"]'  =>  isset($_SERVER['HTTP_CLIENT_IP'])?$_SERVER['HTTP_CLIENT_IP']:'NULL',
            '$_SERVER["REMOTE_ADDR"]'  =>  $_SERVER['REMOTE_ADDR'],
            'getenv("HTTP_X_FORWARDED_FOR")'  =>  getenv('HTTP_X_FORWARDED_FOR'),
            'getenv("HTTP_CLIENT_IP")'  =>  getenv('HTTP_CLIENT_IP'),
            'getenv("REMOTE_ADDR")'  =>  getenv('REMOTE_ADDR'),
        ],
    ];
    $attach and $trace = array_merge($trace,$attach);


    $nav = '';
    $win = '';
    foreach($trace as $key => $value){
        $nav .= "<span style=\"color:#000;padding-right:12px;height:30px;line-height: 30px;display:inline-block;margin-right:3px;cursor: pointer;font-weight:700\">$key</span>";
        $win .= '<div style="display:none;"><ol style="padding: 0; margin:0">';

        if(is_array($value)){
            foreach ($value as $k=>$val){
                if(!is_string($val)) $val = var_export($val,true);
                $win .='<li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px"><span style="color: blue">' .
                    (is_numeric($k) ? '' : $k.':</span>') .
                    "<span  style='color:black'>{$val}</span></li>";
            }
        }else{
            $win .= htmlspecialchars($value,ENT_COMPAT,'utf-8');
        }
        $win .= '</ol></div>';
    }

    echo <<< endline
    <div style="border-left:thin double #ccc;position: fixed;bottom:0;right:0;font-size:14px;width:1280px;z-index: 999999;color: #000;text-align:left;font-family:'Times New Roman';cursor:default;">
        <div id="ptt" style="display: none;background:white;margin:0;height: 512px;">
            <!-- 导航条 -->
            <div id="pttt" style="height:32px;padding: 6px 12px 0;border-bottom:1px solid #ececec;border-top:1px solid #ececec;font-size:16px">
                {$nav}
            </div>
            <!-- 详细窗口 -->
            <div id="pttc" style="overflow:auto;height:478px;padding: 0; line-height: 24px">
                {$win}
            </div>
        </div>
        <!-- 关闭按钮 -->
        <div id="ptc" style="display:none;text-align:right;height:15px;position:absolute;top:10px;right:12px;cursor: pointer;">
            <img style="vertical-align:top;" src="data:image/png;base64,R0lGODlhDwAPAJEAAAAAAAMDA////wAAACH/C1hNUCBEYXRhWE1QPD94cGFja2V0IGJlZ2luPSLvu78iIGlkPSJXNU0wTXBDZWhpSHpyZVN6TlRjemtjOWQiPz4gPHg6eG1wbWV0YSB4bWxuczp4PSJhZG9iZTpuczptZXRhLyIgeDp4bXB0az0iQWRvYmUgWE1QIENvcmUgNS4wLWMwNjAgNjEuMTM0Nzc3LCAyMDEwLzAyLzEyLTE3OjMyOjAwICAgICAgICAiPiA8cmRmOlJERiB4bWxuczpyZGY9Imh0dHA6Ly93d3cudzMub3JnLzE5OTkvMDIvMjItcmRmLXN5bnRheC1ucyMiPiA8cmRmOkRlc2NyaXB0aW9uIHJkZjphYm91dD0iIiB4bWxuczp4bXA9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC8iIHhtbG5zOnhtcE1NPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvbW0vIiB4bWxuczpzdFJlZj0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wL3NUeXBlL1Jlc291cmNlUmVmIyIgeG1wOkNyZWF0b3JUb29sPSJBZG9iZSBQaG90b3Nob3AgQ1M1IFdpbmRvd3MiIHhtcE1NOkluc3RhbmNlSUQ9InhtcC5paWQ6MUQxMjc1MUJCQUJDMTFFMTk0OUVGRjc3QzU4RURFNkEiIHhtcE1NOkRvY3VtZW50SUQ9InhtcC5kaWQ6MUQxMjc1MUNCQUJDMTFFMTk0OUVGRjc3QzU4RURFNkEiPiA8eG1wTU06RGVyaXZlZEZyb20gc3RSZWY6aW5zdGFuY2VJRD0ieG1wLmlpZDoxRDEyNzUxOUJBQkMxMUUxOTQ5RUZGNzdDNThFREU2QSIgc3RSZWY6ZG9jdW1lbnRJRD0ieG1wLmRpZDoxRDEyNzUxQUJBQkMxMUUxOTQ5RUZGNzdDNThFREU2QSIvPiA8L3JkZjpEZXNjcmlwdGlvbj4gPC9yZGY6UkRGPiA8L3g6eG1wbWV0YT4gPD94cGFja2V0IGVuZD0iciI/PgH//v38+/r5+Pf29fTz8vHw7+7t7Ovq6ejn5uXk4+Lh4N/e3dzb2tnY19bV1NPS0dDPzs3My8rJyMfGxcTDwsHAv769vLu6ubi3trW0s7KxsK+urayrqqmop6alpKOioaCfnp2cm5qZmJeWlZSTkpGQj46NjIuKiYiHhoWEg4KBgH9+fXx7enl4d3Z1dHNycXBvbm1sa2ppaGdmZWRjYmFgX15dXFtaWVhXVlVUU1JRUE9OTUxLSklIR0ZFRENCQUA/Pj08Ozo5ODc2NTQzMjEwLy4tLCsqKSgnJiUkIyIhIB8eHRwbGhkYFxYVFBMSERAPDg0MCwoJCAcGBQQDAgEAACH5BAAAAAAALAAAAAAPAA8AAAIdjI6JZqotoJPR1fnsgRR3C2jZl3Ai9aWZZooV+RQAOw==" />
        </div>
    </div>
    <!-- 开启按钮 -->
    <div id="pto" style="height:30px;float:right;text-align: right;overflow:hidden;position:fixed;bottom:0;right:0;color:#000;line-height:30px;cursor:pointer;">
        <div style="background:#232323;color:#FFF;padding:0 6px;float:right;line-height:30px;font-size:14px"></div>
    <img style="width: 30px" title="ShowPageTrace" src="data:image/ico;base64,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AM0AAADOAAAAzgAAAM4AAADOAAAAzwAAAMQAAAAzAAAAAAAAAHAAAADKAAAAEQAAAAAAAAAAAAAAAAAAAHMAAAD/AAAAmgAAAA0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAQAAAAEgAAABIAAAASAAAAEgAAABIAAAASAAAAEgAAABIAAAATAAAAEAAAAAEAAAAOAAAAwwAAAIIAAAAAAAAAAAAAAAAAAAAAAAAAIQAAANkAAABuAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAF8AAADbAAAAKQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAcwAAANcAAAApAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAiAAAA0gAAAH8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAPAAAAtAAAALMAAAASAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADgAAAKoAAAC9AAAAFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAApAAAAzgAAAKMAAAASAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA8AAACaAAAA1AAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA0AAAAzgAAALMAAAApAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAkAAAArAAAANQAAAA8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAApAAAAtQAAANcAAABqAAAAFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAARAAAAZAAAANQAAAC7AAAALwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAPAAAAdAAAANcAAADLAAAAfAAAADgAAAAVAAAABwAAAAIAAAACAAAABgAAABQAAAA2AAAAdwAAAMcAAADZAAAAegAAABIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIgAAAHYAAADCAAAA3QAAANEAAAC7AAAArAAAAKwAAAC6AAAA0AAAAN0AAADFAAAAewAAACUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAwAAAAwAAAAWQAAAHgAAACJAAAAiQAAAHkAAABbAAAAMgAAAA4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA/4AA//wAAD/4AAAf8AAAD/AAAAfwAGAD8AA4AYAAHAGDwA4AA+AHAAPAA4ADgAPAAwABwAYAAeAEAAHgAAGB4AADgeAAAAHgAAAAAAAAAAAAIAAAAGAAAADwAACAf/4BgD/8AcAf+APAB+AD4AAAB/AAAA/4AAAf/gAAf/+AAf8=" />
    </div>
    <script type="text/javascript">
    (function(){
        var dge = function(i){return document.getElementById(i);};
        var tab_tit  = dge('pttt').getElementsByTagName('span');
        var tab_cont = dge('pttc').getElementsByTagName('div');
        var open     = dge('pto');
        var close    = dge('ptc');
        var trace    = dge('ptt');
        var cookie   = document.cookie.match(/_spt_=(\d\|\d)/);
        var history  = (cookie && typeof cookie[1] != 'undefined' && cookie[1].split('|')) || [0,0];
        open.onclick = function(){
            trace.style.display = '';
            close.style.display = '';
            history[0] = 1;
            document.cookie = '_spt_='+history.join('|');
        };
        close.onclick = function(){
            trace.style.display = 'none';
            open.style.display = 'block';
            history[0] = 0;
            document.cookie = '_spt_='+history.join('|');
        };
        for(var i = 0; i < tab_tit.length; i++){
            tab_tit[i].onclick = (function(i){
                return function(){
                    for(var j = 0; j < tab_cont.length; j++){
                        tab_cont[j].style.display = 'none';
                        tab_tit[j].style.color = '#999';
                    }
                    tab_cont[i].style.display = 'block';
                    tab_tit[i].style.color = '#000';
                    history[1] = i;
                    document.cookie = '_spt_='+history.join('|')
                }
            })(i);
        }
        parseInt(history[0]) && open.click();
        (tab_tit[history[1]] || tab_tit[0]).click();
    })();
    </script>
endline;
}
defined('IS_AJAX') or define('IS_AJAX', ((isset($_SERVER['HTTP_X_REQUESTED_WITH']) and strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == 'xmlhttprequest') ));

register_shutdown_function(function (){
    IS_AJAX or showtrace([

    ],[
    ]);
});