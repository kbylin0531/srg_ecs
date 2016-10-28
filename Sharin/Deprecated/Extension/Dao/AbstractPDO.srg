<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2015/8/17
 * Time: 9:47
 */
namespace System\Core\Dao;
defined('BASE_PATH') or die('No Permission!');
/**
 * Class AbstractPDO
 * @package namespace System\Core\Dao;
 *
 * 数据库驱动的具体细节实现类
 * 公共的方法在该类中实现
 * 子类根据具体数据库的不同选择不同的实现的方法在本类中以抽象方法表示
 */
abstract class AbstractPDO extends \PDO{

    /**
     * 保留字段转义字符
     * mysql中是 ``
     * sqlserver中是 []
     * oracle中是 ""
     * @var string
     */
    protected static $_l_quote = null;
    protected static $_r_quote = null;

    /**
     * PDO驱动器名称
     * @var string
     */
    protected $driverName = null;

    /**
     * 禁止访问的PDO函数的名称
     * @var array
     */
    protected $forbidMethods = [
        'forbid','getColumnMeta'
    ];

    /**
     * 创建驱动类对象
     * DatabaseDriver constructor.
     * @param array $config
     */
    public function __construct(array $config){
        $this->driverName = get_class($this);
        parent::__construct($this->buildDSN($config),$config['username'],$config['password'],$config['options']);
    }

    /**
     * 根据配置创建DSN
     * @param array $config 数据库连接配置
     * @return string
     */
    abstract public function buildDSN($config);

    /**
     * 编译组件成适应当前数据库的SQL字符串
     * @param array $components  复杂SQL的组成部分
     * @return string
     */
    abstract public function compile(array $components);

    /**
     * 转义保留字字段名称
     * @param string $fieldname 字段名称
     * @return string
     */
    abstract public function escapeField($fieldname);


    /**
     * 取得数据表的字段信息
     * @access public
     * @param $tableName
     * @return array
     */
    abstract public function getFields($tableName);

    /**
     * 调用不存在的方法时
     * @param string $name 方法名称
     * @param array $args 方法参数
     * @return mixed
     */
    public function __call($name,$args){
        if(in_array($name,$this->forbidMethods,true))  return false;
        return call_user_func_array(array($this,$name),$args);
    }

    /**
     * 创建数据库
     * @param string $dbname 数据库名称
     * @return int 受影响的行数
     */
    abstract public function createDatabase($dbname);

}