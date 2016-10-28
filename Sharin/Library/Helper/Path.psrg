<?php
/**
 * Created by PhpStorm.
 * User: lich4ung
 * Date: 9/14/16
 * Time: 10:44 AM
 */

namespace Sharin\Library\Helper;


class Path {
    /**
     * 判断路径是不是绝对路径
     * @param $path
     * @return bool 返回true('/foo/bar','c:\windows').否则返回false
     */
    public static function isAbsolute($path){
        if (realpath($path) == $path)// *nux 的绝对路径 /home/my
            return true;
        if (strlen($path) == 0 || $path[0] == '.')
            return false;
        if (preg_match('#^[a-zA-Z]:\\\\#', $path))// windows 的绝对路径 c:\aaa\
            return true;
        return (bool)preg_match('#^[/\\\\]#', $path); //绝对路径 运行 / 和 \绝对路径，其他的则为相对路径
    }

}