<?php
/**
 * Powered by linzhv@qq.com.
 * Github: git@github.com:linzongho/sharin.git
 * User: root
 * Date: 16-9-3
 * Time: 上午11:23
 */
namespace Sharin;

//运行环境检测
version_compare(PHP_VERSION,'7','<') and die('Require php >= 7 !');


function _buildMessage($params, $traces)
{
    $color = '#';
    $str = '9ABCDEF';//随机浅色背景
    for ($i = 0; $i < 6; $i++) $color = $color . $str[rand(0, strlen($str) - 1)];
    $str = "<pre style='background: {$color};width: 100%;padding: 10px;margin: 0'><h3 style='color: midnightblue'><b>F:</b>{$traces[0]['file']} << <b>L:</b>{$traces[0]['line']} >> </h3>";
    foreach ($params as $key => $val){
        $txt = var_export($val, true);
        $str .= "<b>Parameter- . $key :</b><br /> $txt <br />";
    }
    return $str . '</pre>';
}

function _buildMessage4Client($params, $traces)
{
    $str = "F:{$traces[0]['file']} << L:{$traces[0]['line']} >>" . PHP_EOL;
    foreach ($params as $key => $val) $str .= "[Parameter-{$key}]\n" . var_export($val, true) . PHP_EOL;
    return $str;
}

/**
 * @param ... it will return all message debugged if sum of parameters is zero
 * @return string|array
 */
function debug()
{
    return '';
}

/**
 * @param ...
 */
function dump()
{
    echo call_user_func_array(SR_IS_CLI?'\Sharin\_buildMessage4Client':'\Sharin\_buildMessage',[
        func_get_args(), debug_backtrace()
    ]);
}

/**
 * @param ...
 * @return void
 */
function dumpout()
{
    echo call_user_func_array(SR_IS_CLI?'\Sharin\_buildMessage4Client':'\Sharin\_buildMessage',[
        func_get_args(), debug_backtrace()
    ]);
    exit();
}
