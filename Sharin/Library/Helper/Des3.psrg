<?php
/**
 * Created by PhpStorm.
 * User: asus
 * Date: 9/16/16
 * Time: 4:22 PM
 */

namespace Sharin\Library\Helper;

/**
 * PHP版3DES加解密类
 * 可与java的3DES(DESede)加密方式兼容
 * @Author: Luo Hui (farmer.luo at gmail.com)
 * @version: V0.1 2008.12.04
 */
class Des3
{

    private static $key    = "01234567890123456789012345678912";
    private static $iv    = "23456789"; //like java: private static byte[] myIV = { 50, 51, 52, 53, 54, 55, 56, 57 };
    //加密
    public static function encrypt($input)
    {
        $input = self::padding( $input );
        $key = base64_decode(self::$key);
        $td = mcrypt_module_open( MCRYPT_3DES, '', MCRYPT_MODE_CBC, '');
        //使用MCRYPT_3DES算法,cbc模式
        mcrypt_generic_init($td, $key, self::$iv);
        //初始处理
        $data = mcrypt_generic($td, $input);
        //加密
        mcrypt_generic_deinit($td);
        //结束
        mcrypt_module_close($td);
        $data = self::removeBR(base64_encode($data));
        return $data;
    }
    //解密
    public static function decrypt($encrypted)
    {
        $encrypted = base64_decode($encrypted);
        $key = base64_decode(self::$key);
        $td = mcrypt_module_open( MCRYPT_3DES,'',MCRYPT_MODE_CBC,'');
        //使用MCRYPT_3DES算法,cbc模式
        mcrypt_generic_init($td, $key, self::$iv);
        //初始处理
        $decrypted = mdecrypt_generic($td, $encrypted);
        //解密
        mcrypt_generic_deinit($td);
        //结束
        mcrypt_module_close($td);
        $decrypted = self::removePadding($decrypted);
        return $decrypted;
    }
    //填充密码，填充至8的倍数
    private static function padding( $str )
    {
        $len = 8 - strlen( $str ) % 8;
        for ( $i = 0; $i < $len; $i++ )
        {
            $str .= chr( 0 );
        }
        return $str ;
    }
    //删除填充符
    private static function removePadding( $str )
    {
        $len = strlen( $str );
        $newstr = "";
        $str = str_split($str);
        for ($i = 0; $i < $len; $i++ )
        {
            if ($str[$i] != chr( 0 ))
            {
                $newstr .= $str[$i];
            }
        }
        return $newstr;
    }
    //删除回车和换行
    private static function removeBR( $str )
    {
        $len = strlen( $str );
        $newstr = "";
        $str = str_split($str);
        for ($i = 0; $i < $len; $i++ )
        {
            if ($str[$i] != '\n' and $str[$i] != '\r')
            {
                $newstr .= $str[$i];
            }
        }
        return $newstr;
    }
}
/**
 * 测试
$rst = Crypt3Des::encrypt('linzhonghuang');
echo $rst;
echo '<br />';
//            echo Crypt3Des::decrypt($rst);
echo '<br />';
 */