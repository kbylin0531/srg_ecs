<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2015/8/17
 * Time: 9:45
 */
namespace System\Core\Dao;
use System\Exception\CoraxException;
use System\Util\SEK;

defined('BASE_PATH') or die('No Permission!');

/**
 * Class MysqlDriver
 * @package System\Core\DaoDriver
 */
class Mysql extends AbstractPDO{

    protected static $_l_quote = '`';
    protected static $_r_quote = '`';

    /**
     * 构造函数
     * @param array $config
     * @throws CoraxException
     */
    public function __construct(array $config){
        //检查扩展是否开启
        if(!SEK::phpExtend('pdo_mysql')){
//            dl('pdo_mysql');
            throw new CoraxException('Please extend pdo_mysql');
        }
        parent::__construct($config);
    }

    /**
     * 编译组件成适应当前数据库的SQL字符串
     * @param array $compos  复杂SQL的组成部分
     * @return string 返回编译后的SQL字符串
     * @throws CoraxException
     */
    public function compile(array $compos){
        $components = [
            'distinct'  =>  null,
            'fields'=>' * ', //查询的表域情况
            'table' => null,
            'join'  => null,     //join部分，需要带上join关键字
            'where' => null, //where部分
            'group' => null, //分组 需要带上group by
            'having'=> null,//having子句，依赖$group存在，需要带上having部分
            'order' => null,//排序，不需要带上order by
            'offset'=> null,
            'limit' => null,
        ];
        $components = array_merge($components,$compos);
        if(!$components['table']){
            throw new CoraxException('Empty table name is invalid!');
        }
        if($components['distinct']){
            $sql = " SELECT DISTINCT {$components['fields']} FROM {$components['fields']} ";
        }else{
            $sql = " SELECT {$components['fields']} FROM {$components['fields']} ";
        }

        //顺序连接
        $components['join']     and $sql    = "{$sql} {$components['join']}";
        $components['where']    and $sql    = "{$sql} WHERE {$components['where']}";
        $components['group']    and $sql    = "{$sql} GROUP BY {$components['group']} ";
        $components['having']   and $sql    = "{$sql} HAVING {$components['having']} ";
        $components['order']    and $sql    = "{$sql} ORDER BY {$components['order']} ";

        if(isset($components['limit'])){
            $sql = isset($components['offset'])?
                "{$sql} LIMIT {$components['offset']},{$components['limit']}" :
                "{$sql} LIMIT {$components['limit']}";
        }
        return "{$sql};";
    }


    public function getTables($namelike = '%',$dbname=null){
        $sql    = isset($dbname)?
            "SHOW TABLES FROM  {$dbname}  LIKE '{$namelike}' ":
            "SHOW TABLES  LIKE '{$namelike}' ";
        $result = $this->query($sql)->fetchAll();
        $info   = [];
        foreach ($result as $key => $val) {
            $info[$key] = current($val);
        }
        return $info;
    }


    public function escapeField($fieldname){
        return self::$_l_quote.trim($fieldname).self::$_r_quote;
    }

    /**
     * @param array $config
     * @return string
     */
    public function buildDSN($config){
        $dsn  =  "mysql:host={$config['host']}";
        $config['dbname']   and $dsn = "{$dsn};dbname={$config['dbname']}";
        $config['port']     and $dsn = "{$dsn};port={$config['port']}";
        $config['socket']   and $dsn = "{$dsn};unix_socket={$config['socket']}";
        $config['charset']  and $dsn = "{$dsn};charset={$config['charset']}";
        return $dsn;
    }

    /**
     * 取得数据表的字段信息
     * @access public
     * @param $tableName
     * @return array
     */
    public function getFields($tableName) {
        list($tableName) = explode(' ', $tableName);
        $sql   = 'SHOW COLUMNS FROM `'.$tableName.'`';
        $result = $this->query($sql);
        $info   =   array();
        if($result) {
            foreach ($result->fetchAll() as $key => $val) {
                $info[$val['field']] = array(
                    'name'    => $val['field'],
                    'type'    => $val['type'],
                    'notnull' => $val['null'] === '', // not null is empty, null is yes
                    'default' => $val['default'],
                    'primary' => (strtolower($val['key']) === 'pri'),
                    'autoinc' => (strtolower($val['extra']) === 'auto_increment'),
                );
            }
        }
        return $info;
    }

    /**
     * 创建数据库
     * @param string $dbname 数据库名称
     * @return int 受影响的行数
     */
    public function createDatabase($dbname){
        $sql = "CREATE DATABASE IF NOT EXISTS `{$dbname}` DEFAULT CHARACTER SET utf8";
        return $this->exec($sql);
    }

}