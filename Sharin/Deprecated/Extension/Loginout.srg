<?php
/**
* Email: linzongho@gmail.com
* Github: https://github.com/linzongho/sharin
* User: asus
* Date: 8/23/16
* Time: 5:17 PM
*/
namespace Sharin\Extension ;
use Sharin\Developer;
use Sharin\Exceptions\Extension\NoModelUsableException;
use Sharin\Extension\Interfaces\LoginoutInterface;
use Sharin\Library\Cookie;
use Sharin\Library\Helper\Base64;
use Sharin\Library\Session;

/**
 * Class Loginout
 * 通用的登录登出工具
 * @package Shirley
 */
class Loginout {

    /**
     * @var LoginoutInterface 登录登出器
     */
    private static $model = null;
    /**
     * @var array 用户的登录信息
     */
    private static $info = null;
    /**
     * @var string
     */
    private static $username = null;

    /**
     * 获取session和cookie名称，兼加密密钥
     * @static
     * @return string|false
     */
    private static function getKey(){
        return 'LOGIN_'.SR_APP_NAME;
    }
    /**
     * 获取当前登录的账户的信息
     * @param string|null $tname 为null时获取全部信息
     * @return array|null 信息不存在时返回null
     */
    public static function getUserinfo($tname=null){
        if(!self::$info){
            if(self::$info = self::_getInfoFromSessionOrCookie()){
                Developer::trace('load Login info from session or cookie!');
            }else{
                return null;
            }
        }
        return $tname? (isset(self::$info[$tname])?self::$info[$tname]:null) : self::$info;
    }

    private static function _getInfoFromSessionOrCookie(){
        $key = self::getKey();
        $info = Session::get($key);//return null if not set
        if(!$info){
            //未登录时检查cookie中是否记录账户要求rememeber的未过期的信息
            $cookie = Cookie::get($key);
            if($cookie){
                $info = unserialize(Base64::decrypt($cookie, $key));
                Session::set($key, $info);
            }else{
                return null;
            }
        }
        return self::$info = $info;
    }

    /**
     * 检查在该场景中用户是否处于登录状态
     * @static
     * @return bool
     */
    public static function check(){
        if(!self::$info){
            //如果设置了info，则一定是有效的登录
            $key = self::getKey();
            $status = Session::get($key);//return null if not set
            if(!$status){
                //未登录时检查cookie中是否记录账户要求rememeber的未过期的信息
                $cookie = Cookie::get($key);
                if($cookie){
                    Session::set($key, self::$info = unserialize(Base64::decrypt($cookie, $key)));
                }else{
                    return false;
                }
            }else{
                self::$info = $status;
            }
        }
        return true;
    }

    /**
     * 执行登录操作
     * @param string $username 用户名
     * @param string $password 密码
     * @param LoginoutInterface $model
     * @return true|string 登录成功时返回true，否则返回错误信息
     * @throws NoModelUsableException
     */
    public static function login($username,$password,LoginoutInterface $model=null){
        $model and self::$model = $model;
        if(self::$model instanceof LoginoutInterface) {
            $info = self::$model->login($username,$password);
            if(is_string($info)){
                return $info;
            }else{
                $key = self::getKey();
                Session::set($key, $info);
                self::$username = $username;
                self::$info = $info;
                return true;
            }
        }else{
            throw new NoModelUsableException($model);
        }
    }

    /**
     * 记住用户的登录信息
     * @static
     * @param int $expire
     * @param null $info
     * @return void
     */
    public static function remember($expire=ONE_WEEK,$info=null){
        $info or $info = self::getUserinfo();
        $key = self::getKey();
        Cookie::set($key, Base64::encrypt(serialize($info), $key), $expire);
    }

    /**
     * 注销登陆
     * @param LoginoutInterface $model
     * @return bool
     */
    public static function logout(LoginoutInterface $model=null){
        $model and self::$model = $model;
        if(self::$model and !self::$model->logout()){
            return false;/* 设置了model并且model登出失败 */
        }
        $key = self::getKey();
        Session::delete($key);
        Cookie::clear($key);
        return true;
    }
}