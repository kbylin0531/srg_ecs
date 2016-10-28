<?php

/**
 * Email: linzongho@gmail.com
 * Github: https://github.com/linzongho/Sharin
 * User: asus
 * Date: 8/24/16
 * Time: 9:55 AM
 */
namespace Sharin\Exceptions\Database;

class ConnectFailedException extends \Exception {
    /**
     * ConnectFailedException constructor.
     * @param string $dsn
     * @param array $config
     * @param string $syserrorinfo 相关驱动报的错误信息
     */
    public function __construct($dsn,array $config,$syserrorinfo='') {
        if(SR_DEBUG_MODE_ON){
            $info = var_export($config,true);
            $this->message = "Connect failed with DSN'{$dsn}' and '{$info}' ... {{$syserrorinfo}} ";
        }else{
            $this->message = 'Database is busy now!';
        }
    }

}