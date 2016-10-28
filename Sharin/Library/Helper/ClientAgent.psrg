<?php
/**
 * Powered by linzhv@qq.com.
 * Github: git@github.com:linzongho/sharin.git
 * User: root
 * Date: 16-9-3
 * Time: 下午6:24
 */

namespace Sharin\Library\Helper;

class ClientAgent {

    /**
     * 浏览器类型
     */
    const AGENT_IE      = 'ie';
    const AGENT_FIRFOX  = 'firefox';
    const AGENT_CHROME  = 'chrome';
    const AGENT_OPERA   = 'opera';
    const AGENT_SAFARI  = 'safari';
    const AGENT_UNKNOWN = 'unknown';


    const LANG_ZH       = 'zh';
    const LANG_ZH_CN    = 'zh_CN';
    const LANG_ZH_TW    = 'zh_TW';
    const LANG_EN       = 'en';
    const LANG_EN_US    = 'en_US';

    /**
     * 获取浏览器类型
     * @return string
     */
    public static function getBrowser(){
        if (empty($_SERVER['HTTP_USER_AGENT'])){    //当浏览器没有发送访问者的信息的时候
            return 'unknow';
        }
        $agent=$_SERVER["HTTP_USER_AGENT"];
        if(strpos($agent,'MSIE')!==false || strpos($agent,'rv:11.0')) //ie11判断
            return self::AGENT_IE;
        else if(strpos($agent,'Firefox')!==false)
            return self::AGENT_FIRFOX;
        else if(strpos($agent,'Chrome')!==false)
            return self::AGENT_CHROME;
        else if(strpos($agent,'Opera')!==false)
            return self::AGENT_OPERA;
        else if((strpos($agent,'Chrome')==false)&&strpos($agent,'Safari')!==false)
            return self::AGENT_SAFARI;
        else
            return self::AGENT_UNKNOWN;
    }

    /**
     * get language from client
     * @param string $default
     * @return string
     */
    public static function getClientLang($default='en'){
        $matches = [];
        if(isset($_SERVER['HTTP_ACCEPT_LANGUAGE'])){
            preg_match('/^([a-z\-]+)/i', $_SERVER['HTTP_ACCEPT_LANGUAGE'], $matches);
            if(!empty($matches[1])){
                switch (substr($matches[1],0,2)) {
                    case 'zh':
                        //忽略新加坡的简体和香港的繁体
                        if ($matches[1] != 'zn-TW'){
                            $matches[1] = 'zh-CN';
                        }
                        break;
                    //default to 'en'
                    case 'en':
                    default:
                        $matches[1] = 'en';break;
                }
                return str_replace('-', '_',$matches[1]);
            }
        }
        return $default;
    }

    /**
     * 获取浏览器版本
     * @return string
     */
    public static function getBrowserVer(){
        if (empty($_SERVER['HTTP_USER_AGENT'])){    //当浏览器没有发送访问者的信息的时候
            return self::AGENT_UNKNOWN;
        }
        $agent= $_SERVER['HTTP_USER_AGENT'];
        if (preg_match('/MSIE\s(\d+)\..*/i', $agent, $regs))
            return $regs[1];
        elseif (preg_match('/FireFox\/(\d+)\..*/i', $agent, $regs))
            return $regs[1];
        elseif (preg_match('/Opera[\s|\/](\d+)\..*/i', $agent, $regs))
            return $regs[1];
        elseif (preg_match('/Chrome\/(\d+)\..*/i', $agent, $regs))
            return $regs[1];
        elseif ((strpos($agent,'Chrome')==false) and preg_match('/Safari\/(\d+)\..*$/i', $agent, $regs))
            return $regs[1];
        else
            return self::AGENT_UNKNOWN;
    }
    public static function is_ip($str){
        $ip = explode('.', $str);
        for ($i = 0; $i < count($ip); $i++) {
            if ($ip[$i] > 255) {
                return false;
            }
        }
        return preg_match('/^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$/',$str);
    }
    /**
     * 获取客户端IP地址
     * 获取IP地址（摘自discuz）
     * @return mixed
     */
    public static function getClientIP() {
        if(isset($_SERVER)){
            if(isset($_SERVER['HTTP_X_FORWARDED_FOR'])){
                $realip=$_SERVER['HTTP_X_FORWARDED_FOR'];
            }else if(isset($_SERVER['HTTP_CLIENT_IP'])){
                $realip=$_SERVER['HTTP_CLIENT_IP'];
            }else{
                $realip=$_SERVER['REMOTE_ADDR'];
            }
        }else{
            if(getenv('HTTP_X_FORWARDED_FOR')){
                $realip=getenv('HTTP_X_FORWARDED_FOR');
            }else if(getenv('HTTP_CLIENT_IP')){
                $realip=getenv('HTTP_CLIENT_IP');
            }else{
                $realip=getenv('REMOTE_ADDR');
            }
        }
        return $realip;
    }

    /**
     * 确定客户端发起的请求是否基于SSL协议
     * @return bool
     */
    public static function isHttps(){
        return (isset($_SERVER['HTTPS']) and ('1' == $_SERVER['HTTPS'] or 'on' == strtolower($_SERVER['HTTPS']))) or
        (isset($_SERVER['SERVER_PORT']) and ('443' == $_SERVER['SERVER_PORT']));
    }

    public static function isPhone(){
        if(isset($_SERVER['HTTP_USER_AGENT']) and preg_match('/(up.browser|up.link|mmp|symbian|smartphone|midp|wap|phone|iphone|ipad|ipod|android|xoom)/i',
                strtolower($_SERVER['HTTP_USER_AGENT']))){
            return true;
        }elseif((isset($_SERVER['HTTP_ACCEPT'])) and (strpos(strtolower($_SERVER['HTTP_ACCEPT']),'application/vnd.wap.xhtml+xml') !== false)){
            return true;
        }
        return false;
    }
}