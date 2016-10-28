#!/usr/bin/env php
<?php
namespace {

    use Workerman\Autoloader;
    use Workerman\Worker;
    use Workerman\WebServer;

    require __DIR__.'/Common/constant.inc';
    require __DIR__.'/Common/debug_suit.inc';
    require __DIR__.'/Plugin/Workerman/Autoloader.php';

    //error  display
    if(SR_DEBUG_MODE_ON){
        error_reporting(-1);
        ini_set('display_errors',1);
    }else{
        error_reporting(E_ALL & ~E_NOTICE & ~E_DEPRECATED & ~E_STRICT & ~E_USER_NOTICE & ~E_USER_DEPRECATED);//php5.3version use code: error_reporting(E_ALL & ~E_NOTICE & ~E_STRICT & ~E_USER_NOTICE);
        ini_set('display_errors', 0);
    }

    /**
     * Class Service
     */
    final class Service {
        /**
         * @var Worker[]
         */
        private static $workers = [];
        /**
         * @var WebServer[]
         */
        private static $webservers = [];

        /**
         * 命令行参数管理
         * @param int $index
         * @param mixed $replacement
         * @return mixed
         */
        public static function arg($index=1,$replacement=''){
            global $argv;
            return isset($argv[$index])? $argv[$index] : $replacement;
        }

        /**
         * 开启应用
         */
        public static function start(){
            $entry = SR_PATH_APP.DIRECTORY_SEPARATOR.'start.php';
            if(is_file($entry)){
                include $entry;
            }else{
                die("Service script '$entry' is not a file!\n");
            }
        }

        /**
         * Create a Websocket server
         * @param string $socketName
         * @param array $contextOpts
         * @return Worker
         */
        public static function getWorker($socketName = '', array $contextOpts = []){
            if(empty(self::$workers[$socketName])){
                self::$workers[$socketName] = new Worker($socketName,$contextOpts);
            }
            return self::$workers[$socketName];
        }

        /**
         * @param string $socketName
         * @param array $contextOpts
         * @return WebServer
         */
        public static function getWebServer($socketName = '', array $contextOpts = []){
            if(empty(self::$webservers[$socketName])){
                self::$webservers[$socketName] = new WebServer($socketName,$contextOpts);
            }
            return self::$webservers[$socketName];
        }

        /**
         * 运行环境检查
         * @return bool|string
         */
        public static function checkEnv(){
            global $argv;
            //补充service.php占据的位置
            if(isset($argv[2])){
                $argv[1] = $argv[2];
                echo "ARG-1:{$argv[1]} \n";
            }
            if(isset($argv[3])){
                $argv[2] = $argv[3];
                echo "ARG-2:{$argv[2]} \n";
            }
            if(isset($argv[4])) {
                $argv[3] = $argv[4];
                echo "ARG-3:{$argv[3]} \n";
            }
            $error = '';
            // 检查扩展
            if(!extension_loaded('pcntl')) $error = "Please install pcntl extension. See http://doc3.workerman.net/install/install.html\n";
            if(!extension_loaded('posix')) $error = "Please install posix extension. See http://doc3.workerman.net/install/install.html\n";
            return $error;
        }
    }


    //开启脚本
    $appname = Service::arg(1);
    if(!$appname){
        die("Please run as 'service.go [APPLICATION_NAME] [ACTION_NAME]'!\n");
    }
    define('SR_APP_NAME', ucfirst($appname));
    define('SR_PATH_APP', SR_PATH_SERVICE.DIRECTORY_SEPARATOR.SR_APP_NAME);
    spl_autoload_register([Autoloader::class,'loadByNamespace']);
    if($error = Service::checkEnv()) die($error);
    Service::start();

}