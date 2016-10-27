<?php

/* 错误显示 */
ini_set('display_errors', 1);
error_reporting(E_ALL ^ E_NOTICE);

/* 如果在一个脚本中多次检查同一个文件，而该文件在此脚本执行期间有被删除或修改的危险时，你需要清除文件状态缓存 */
clearstatcache();

/* 定义根目录 */
define('ROOT_PATH', str_replace('install/includes/init.php', '', str_replace('\\', '/', __FILE__)));
define('PHP_SELF', isset($_SERVER['PHP_SELF']) ? $_SERVER['PHP_SELF'] : $_SERVER['SCRIPT_NAME']);
//var_dump($_SERVER,PHP_SELF);die();//访问的相对目录：/ecshop/install/index.php

/* 定义编码格式 */
define('EC_CHARSET','utf-8');
define('EC_DB_CHARSET','utf8');

/* 函数包加载 */
require(ROOT_PATH . 'includes/lib_base.php');
require(ROOT_PATH . 'includes/lib_common.php');
require(ROOT_PATH . 'includes/lib_time.php');

/* 错误处理类 载入 */
require(ROOT_PATH . 'includes/cls_error.php');
$err = new ecs_error('message.dwt');

/* 模板类 载入 */
require(ROOT_PATH . 'install/includes/cls_template.php');
$smarty = new template(ROOT_PATH . 'install/templates/');

/* 安装相关函数包 载入 */
require(ROOT_PATH . 'install/includes/lib_installer.php');
/* UTF-8 相应头部*/
header('Content-type: text/html; charset='.EC_CHARSET);

set_time_limit(360);