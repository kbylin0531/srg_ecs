<?php
/**
 * 无论是客户端模式还是web模式通用的常量定义在此文件中
 */
const SR_VERSION        = 0;
const SHARIN_VERSION    = 1;
//Component类的重要属性
const BASE_ON_DRIVER        = 'BASE_ON_DRIVER';
const BASED_DRIVER_INDEX    = 'BASED_DRIVER_INDEX';
const DRIVER_CLASS_LIST     = 'DRIVER_CLASS_LIST';
const DRIVER_CONFIG_LIST    = 'DRIVER_CONFIG_LIST';

//数据类型
const TYPE_BOOL     = 'boolean';
const TYPE_INT      = 'integer';
const TYPE_FLOAT    = 'double';//double ,  float
const TYPE_STR      = 'string';
const TYPE_ARRAY    = 'array';
const TYPE_OBJ      = 'object';
const TYPE_RESOURCE = 'resource';
const TYPE_NULL     = 'NULL';
const TYPE_UNKNOWN  = 'unknown type';
//返回直类型
const AJAX_JSON     = 0;
const AJAX_XML      = 1;
const AJAX_STRING   = 2;
//时间常量
const ONE_DAY       = 86400;
const ONE_WEEK      = 604800;
const ONE_MONTH     = 2592000;
//数据库操作类型
const ACTION_SELECT = 0;//查询操作,将使用到$_fields和$_where字段
const ACTION_CREATE = 1;//添加操作,将使用到$_fields字段
const ACTION_UPDATE = 2;//更新操作,将使用到$_fields和$_where字段
const ACTION_DELETE = 3;//删除操作,将使用到$_where字段
//其他
const DS = DIRECTORY_SEPARATOR;

const SR_IS_CLI = PHP_SAPI === 'cli';
define('SR_IS_WIN',     false !== stripos(PHP_OS, 'WIN'));//const IS_WINDOWS = PHP_OS === 'WINNT';
defined('SR_DEBUG_MODE_ON') or define('SR_DEBUG_MODE_ON', true);//可以提高响应速度
defined('SR_OS_ENCODING')   or define('SR_OS_ENCODING','UTF-8');//系统编码

//目录常量
define('SR_PATH_BASE', dirname(dirname(__DIR__)).'/');
const SR_PATH_FRAMEWORK = SR_PATH_BASE.'Sharin/';
const SR_PATH_CONFIG    = SR_PATH_BASE.'Config/';//global config
const SR_PATH_RUNTIME   = SR_PATH_BASE.'Runtime/';
const SR_PATH_PUBLIC    = SR_PATH_BASE.'Public/';
const SR_PATH_SERVICE   = SR_PATH_BASE.'Service/';
const SR_PATH_DATA      = SR_PATH_BASE.'Data/';
const SR_PATH_PLUGIN    = SR_PATH_FRAMEWORK.'Plugins/';
const SR_PATH_APP       = SR_PATH_BASE.'Application/';

//------------------------- 应用生命周期内的各个阶段 --------------------------------------//
const ON_INIT       = 'onInit';
const ON_INITED     = 'onInited';
const ON_START      = 'onStart';
const ON_ROUTE      = 'onRoute';
const ON_CHECK      = 'onCheck';
const ON_DISPATCH   = 'onDispatch';
const ON_STOP       = 'onStop';//应用停止运行的阶段
const ON_SHUTDOWN   = 'onShutdown';
//特殊阶段
const ON_EXCEPTION  = 'onException';
const ON_EXCEPTION_END  = 'onExceptionEnd';
const ON_ERROR      = 'onError';
const ON_ERROR_END  = 'onErrorEnd';
//显示
const ON_ASSIGN         = 'onAssign';
const ON_DISPLAY_BEGIN = 'onDisplayBegin';
const ON_DISPLAY_END   = 'onDisplayEnd';

//------------------------- 语言包 --------------------------------------//
const LANG_ZH_CN = 'zh-cn';
const LANG_ZH_TW = 'zh-tw';
const LANG_EN_US = 'en-us';