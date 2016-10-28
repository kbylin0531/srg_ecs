<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2015/12/20
 * Time: 11:14
 */
namespace System\Core\Dao;

class Oci extends AbstractPDO {
    /**
     * @param array $config
     * @return string
     */
    public function buildDSN($config){
        $dsn  =   'oci:dbname=//'.$config['hostname'].($config['port']?':'.$config['port']:'').'/'.$config['dbname'];
        if(!empty($config['charset'])) {
            $dsn  .= ';charset='.$config['charset'];
        }
        return $dsn;
    }
    public function buildSqlByComponent($tablename,$componets=[],$offset,$limit){}
    public function getTables($namelike = '%', $dbname = null)
    {
        // TODO: Implement getTables() method.
    }

    public function getFields($tableName)
    {
        // TODO: Implement getFields() method.
    }

    public function escapeField($fieldname)
    {
        // TODO: Implement escapeField() method.
    }

    public function buildSql($tablename, array $components, $offset = NULL, $limit = NULL)
    {
        // TODO: Implement buildSql() method.
    }

    public function createDatabase($dbname)
    {
        // TODO: Implement createDatabase() method.
    }
}