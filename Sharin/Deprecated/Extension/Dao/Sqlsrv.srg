<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2015/12/20
 * Time: 11:13
 */
namespace System\Core\Dao;

class Sqlsrv extends AbstractPDO {
    /**
     * @param array $config
     * @return string
     */
    public function buildDSN($config){
        $dsn  =   'sqlsrv:Server='.$config['hostname'];
        if(isset($config['dbname'])){
            $dsn = ";Database={$config['dbname']}";
        }
        if(!empty($config['hostport'])) {
            $dsn  .= ','.$config['hostport'];
        }
        return $dsn;
    }

    /**
     * 根据条件获得查询的SQL，SQL执行的正确与否需要实际查询才能得到验证
     * @param string $tablename 查找的表名称,不需要带上from部分
     * @param array $compos  复杂SQL的组成部分
     * @param null|integer $offset 偏移
     * @param null|integer $limit  选择的最大的数据量
     * @return string 返回组装好的SQL
     */
    public function buildSqlByComponent($tablename,$compos=[],$offset,$limit){
        $components = array(
            'distinct'=>'',
            'top' => '',
            'fields'=>' * ', //查询的表域情况
            'join'=>'',     //join部分，需要带上join关键字
            'where'=>'', //where部分
            'group'=>'', //分组 需要带上group by
            'having'=>'',//having子句，依赖$group存在，需要带上having部分
            'order'=>'',//排序，不需要带上order by
        );
        $components = array_merge($components,$compos);
        if($components['distinct']){//为true或者1时转化为distinct关键字
            $components['distinct'] = 'distinct';
        }
        $sql = " select {$components['distinct']} {$components['top']} {$components['fields']}  from  {$tablename} ";

        //group by，having 加上关键字(对于如group by的组合关键字，只要判断第一个是否存在)如果不是以该关键字开头  则自动添加
        if($components['where'] && 0 !== stripos(trim($components['where']),'where')){
            $components['where'] = ' where '.$components['where'];
        }
        if($components['group'] && 0 !== stripos(trim($components['group']),'group')){
            $components['group'] = ' group by '.$components['group'];
        }
        if( $components['having'] && 0 !== stripos(trim($components['having']),'having')){
            $components['having'] = ' having '.$components['having'];
        }
        //去除order by
        $components['order'] = preg_replace_callback('|order\s*by|i',function(){return '';},$components['order']);

        //按照顺序连接，过滤掉一些特别的参数
        foreach($components as $key=>&$val){
            //$components得分顺序中join开始连接
            if(in_array($key,array('fields','order','top','distinct'))) continue;
            $sql .= " {$val} ";
        }

        $flag = true;//标记是否需要再次设置order by

        //是否偏移
        if(NULL !== $offset && NULL !== $limit){
            $outerOrder = ' order by ';
            if(!empty($components['order'])){
                //去掉其中的order by
                $orders = @explode(',',$components['order']);//分隔多个order项目

                foreach($orders as &$val){
                    $segs = @explode('.',$val);
                    $outerOrder .= array_pop($segs).',';
                }
                $outerOrder  = rtrim($outerOrder,',');
            }else{
                $outerOrder .= ' rand() ';
            }
            $endIndex = $offset+$limit;
            $sql = "SELECT T1.* FROM (
            SELECT  ROW_NUMBER() OVER ( {$outerOrder} ) AS ROW_NUMBER,thinkphp.* FROM ( {$sql} ) AS thinkphp
            ) AS T1 WHERE (T1.ROW_NUMBER BETWEEN 1+{$offset} AND {$endIndex} )";
            $flag = false;
        }
        if($flag && !empty($components['order'])){
            $sql .= ' order by '.$components['order'];
        }
        return $sql;
    }

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