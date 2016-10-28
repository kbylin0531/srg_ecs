<?php
/**
 * Created by PhpStorm.
 * User: asus
 * Date: 9/16/16
 * Time: 4:50 PM
 */

namespace Sharin\Extension;



/**
 * Class CoraxDao 数据入口对象(Data Access Object)
 * 具体方法的实现以来于各个驱动
 * @package System\Core
 */
class CoraxDao{
    /**
     * 数据库类型
     * 具体驱动的名称要根据驱动类型的名称而定
     */
    const DB_TYPE_MYSQL = 'Mysql';
    const DB_TYPE_SQLSRV = 'Sqlsrv';
    const DB_TYPE_ORACLE = 'Oci';//Oracle Call Interface

    /**
     * 保存数据库的驱动，在类构造的过程中初始化
     * @var AbstractPDO
     */
    public $driver;

    /**
     * 指向当前的PDOStatement对象
     * @var \PDOStatement
     */
    protected $curStatement = null;
    /**
     * SQL执行发生的错误信息
     * @var mixed
     */
    protected $error = null;

    /**
     * 实例共用的配置信息
     *  PDO连接信息必须包含： host  username  password [port]
     * @var array
     */
    protected static $convention = [
        //是否启用读写分离
        'IO_SPLITTING_ON'   => false,
        //主库，未选择数据库时默认选择这个连接
        'MASTER_CONF' => [
            'type'      =>  Dao::DB_TYPE_MYSQL,//数据库类型
            'dbname'    => 'corax',//选择的数据库
            'username'  => 'root',
            'password'  => '123456',
            'host'      => 'localhost',
            'port'      => '3306',
            'charset'   => 'UTF8',
            'dsn'       => null,//默认先检查差DSN是否正确,直接写dsn而不设置其他的参数可以提高效率，也可以避免潜在的bug
            'options'   => [],
        ],
        //默认的PDO连接配置
        'PDO_DEFAULT_OPTION'    => [
            \PDO::ATTR_ERRMODE => \PDO::ERRMODE_EXCEPTION,//默认异常模式
            \PDO::ATTR_AUTOCOMMIT => true,//为false时，每次执行exec将不被提交
            \PDO::ATTR_EMULATE_PREPARES => false,//不适用模拟预处理
            \PDO::ATTR_DEFAULT_FETCH_MODE => \PDO::FETCH_ASSOC,//结果集返回形式
        ],
        //从连接库
        'SLAVE_CONFS'    =>   [],
    ];
    /**
     * 数据库连接池
     * 刀池
     * @var array
     */
    protected static $daoPool = [];

    /**
     * 记录类是否已经初始化过
     * @var bool
     */
    protected static $_hasInited = false;

    /**
     * Dao构造函数
     * @param array $config 数据库连接参数
     * @throws CoraxException
     */
    public function __construct(array $config=null){
        //检查类初始化
        self::$_hasInited or self::init();

        //如果未设置将加载默认配置
        if(!isset($config)){
            $config = self::$convention['MASTER_CONF'];
        }elseif(is_array($config)){
            //检查必要参数
            if(!isset($config['type'],$config['username'],$config['password'])){
                throw new CoraxException("The connect config needs 'type','username','password' at least");
            }
        }elseif(is_string($config) or is_numeric($config)){
            if(isset(self::$convention['SLAVE_CONFS'][$config])){
                $config = self::$convention['SLAVE_CONFS'][$config];
            }else{
                throw new CoraxException("Can not find the connect congfig named '{$config}'");
            }
        }else{
            throw new CoraxException('Invalid parameter!');
        }

        //默认的PDO连接配置
        empty($config['options']) and $config['options'] = self::$convention['PDO_DEFAULT_OPTION'];

        //DAO的驱动实例
        $classname = 'System\\Core\\Dao\\'.$config['type'];
        try{
            $this->driver = new $classname($config);
        }catch (\Exception $e){
            //连接失败总会抛出异常PDOException,为避免数据库信息泄露
            Log::debug($e);
            throw new CoraxException('Database connect failed!');
        }
    }
    /*TODO:静态方法 *******************************************************************************/
    /**
     * 初始化实例
     * @param string $confnm 配置名称，存放在根目录下的Configure/文件夹下
     * @return void
     */
    public static function init($confnm='database'){
        if(false === self::$_hasInited){
            $config = Configer::load($confnm);
            SEK::merge(static::$convention,$config);//动态配置载入
            self::$_hasInited = true;
        }
    }

    /**
     * 创建数据库连接
     * 当设置参数二时，将选用参数二作为连接配置创建连接并命名为参数一表示的标识符
     * @param int|string|array $identifier 连接标识符，可以自己创建或者选自self::$_conf['DB_CONNECT']
     *                  如果参数一时数组，则被认为是临时配置，不会被记录，需要用户自己保存好句柄
     * @return Dao
     * @throws CoraxException
     */
    public static function getInstance($identifier=null){
        self::$_hasInited or self::init();
        if(isset(self::$daoPool[$identifier])){
            return self::$daoPool[$identifier];
        }

        //获取配置信息
        if(null === $identifier){
            $connect_config = self::$convention['MASTER_CONF'];
        }elseif(is_array($identifier)){//新建一个连接
            $connect_config = $identifier;
        }elseif(isset(self::$convention['SLAVE_CONFS'][$identifier])){
            $connect_config = self::$convention['SLAVE_CONFS'][$identifier];
        }else{
            throw new CoraxException("Invalid config of connection {$identifier}!");
        }
        return self::$daoPool[$identifier] = new Dao($connect_config);
    }

    /**
     * 获取所有可用的数据库PDO驱动
     * @return array
     */
    public static function getAvailableDrivers(){
        return \PDO::getAvailableDrivers();
    }

    /**
     * 记录查询的SQL
     * @param bool|false $sql
     *      false表示获取最后一条SQL记录(不存在则返回false)，
     *      true表示返回全部记录
     *      string表示设置sql记录，输入参数见bind
     *      array表示设置sql记录，无视参数二
     * @param array|null $bind 输入参数
     * @return array|mixed|null
     * @throws CoraxException
     */
    public static function log($sql=false,array $bind=null){
        static $_cache = array();
        if(is_bool($sql)){
            if($sql){
                return $_cache;
            }else{
                return end($_cache);//默认返回最后一个
            }
        }elseif(is_array($sql)){
            $_cache[] = $sql;
        }elseif(is_string($sql)){
            $_cache[] = array($sql,$bind);
        }else{
            throw new CoraxException($sql,$bind);
        }
        return true;
    }



    /*TODO:基本的查询功能 ***************************************************************************************/

    /**
     * 简单地查询一段SQL，并且将解析出所有的结果集合
     * @param string $sql
     * @return array|false
     */
    public function query($sql){
        self::log($sql);
        $rst = $this->driver->query($sql);
        if(false === $rst){
            //返回false时表示出错
            $this->error = $this->getPdoErrorInfo();
            return false;
        }else{
            //query成功时返回PDOStatement对象
            return $rst->fetchAll();
        }
    }
    /**
     * 简单地执行Insert、Delete、Update操作
     * @param string $sql
     * @return int|false 返回受到影响的行数，但是可能不会太可靠，需要用===判断返回值是0还是false
     *                   放回false表示了错误
     * @throws \PDOException
     */
    public function exec($sql){
        self::log($sql);
        try{
            $rst = $this->driver->exec($sql);
            if(false === $rst){
                $this->error = $this->getPdoErrorInfo();
                return false;
            }
            return $rst;
        }catch (\PDOException $e){
            $this->error = "exec sql of '{$sql}' failed!";
            return false;
        }
    }
    /*TODO:高级查询功能 ***************************************************************************************/

    /**
     * 准备一段SQL
     *  <note>
     *      prepare('insert *****',$id='helo');  准备一段SQL并命名ID为helo
     *      prepare( null|false|''|0 ,$id='helo');  切换到该ID下，并将PDOStatement返回
     *      prepare('insert *****');  将SQL语句设置ID为0并默认指向0
     *  </note>
     * @param string $sql 查询的SQL，当参数二指定的ID存在，只有在参数一布尔值不为false时，会进行真正地prepare
     * @param array $option prepare方法参数二
     * @return $this
     */
    public function prepare($sql,$option=array()){
        $this->curStatement = $this->driver->prepare($sql,$option);
        return $this;
    }

    /**
     * 执行查询功能，返回的结果是bool表示是否执行成功
     * @param array|null $input_parameters
     *                  一个元素个数和将被执行的 SQL 语句中绑定的参数一样多的数组。所有的值作为 PDO::PARAM_STR 对待。
     *                  不能绑定多个值到一个单独的参数,如果在 input_parameters 中存在比 PDO::prepare() 预处理的SQL 指定的多的键名，
     *                  则此语句将会失败并发出一个错误。(这个错误在PHP 5.2.0版本之前是默认忽略的)
     * @param \PDOStatement|null $statement
     * @return bool false时
     * @throws CoraxException
     */
    public function execute(array $input_parameters = null, \PDOStatement $statement=null){
        isset($statement) and $this->curStatement = $statement;
        if(!$this->curStatement) throw new CoraxException('No avalible PDOStatement to execute!');

        self::log($this->curStatement->queryString,$input_parameters);

        //出错时设置错误信息，注：PDOStatement::execute返回bool类型的结果
        if(false === $this->curStatement->execute($input_parameters)){
            $this->error = $this->getStatementErrorInfo();
            return false;
        }
        return true;
    }


    /**
     * 绑定一个参数到指定的变量名
     * 绑定一个PHP变量到用作预处理的SQL语句中的对应命名占位符或问号占位符。
     *      不同于 PDOStatement::bindValue() ，此变量作为引用被绑定，
     *      并只在 PDOStatement::execute() 被调用的时候才取其值
     * <note>
     *      ①如果要使用like查询，%的位置应该在变量处而非SQL语句中
     *      ②foreach ($params as $key => &$val) { $sth->bindParam($key, $val); }时正确的
     *        foreach ($params as $key => $val) { $sth->bindParam($key, $val); }会失败，因为bingParam参数二明确要求是引用变量
     *      ③在MySQL中经过绑定参数，值得类型会发生改变
     *          $active = 1;
     *          $active === 1; //is true
     *          $ps->bindParam(":active", $active, PDO::PARAM_INT);
     *          $ps->execute();
     *          $active === 1;//  will be false
     *      ④一个值对应多个位置在PHP5.2.0及之前的版本中会导致错误，在5.2.1版本之后貌似能正常工作
     *          $sql = "SELECT * FROM u WHERE a = :myValue AND d = :myValue ";
     *          $params = array("myValue" => "0");
     * </note>
     * @param int|string $parameter 参数标识符。
     *                          对于使用命名占位符的预处理语句，应是类似 :name 形式的参数名。
     *                          对于使用问号占位符的预处理语句，应是以1开始索引的参数位置。
     * @param mixed $variable 绑定到 SQL 语句参数的 PHP 变量名
     * @param int $data_type 使用 PDO::PARAM_* 常量明确地指定参数的类型。
     *                       要从一个存储过程中返回一个 INOUT 参数，需要为 data_type 参数使用按位或操作符去设置 PDO::PARAM_INPUT_OUTPUT 位。
     * @param int $length 数据类型的长度。为表明参数是一个存储过程的 OUT 参数，必须明确地设置此长度
     * @param mixed $driver_options 驱动的可选参数
     * @return bool 成功时返回 TRUE， 或者在失败时返回 FALSE。
     */
    public function bindParam($parameter, &$variable, $data_type = \PDO::PARAM_STR, $length = null, $driver_options = null){
        return $this->curStatement->bindParam($parameter,$variable,$data_type,$length,$driver_options);
    }

    /**
     * 绑定一个值到用作预处理的 SQL 语句中的对应命名占位符或问号占位符
     *  参数一和三的意义同bindParam，参数二的意义类似，只是bindValue传递的是值，而非引用
     * <note>
     *      ①由于参数二传递的是值，所以类似一下的调用可以通过，而相同的参数bindParam方法是不通过的
     *          $stmt->bindValue(":something", "bind this");
     * </note>
     * @param mixed $parameter 参数标识符。对于使用命名占位符的预处理语句，应是类似 :name 形式的参数名。对于使用问号占位符的预处理语句，应是以1开始索引的参数位置。
     * @param mixed $value
     * @param int $data_type
     * @return bool 成功时返回 TRUE， 或者在失败时返回 FALSE
     */
    public function bindValue($parameter, $value, $data_type = \PDO::PARAM_STR){
        return $this->curStatement->bindValue($parameter, $value, $data_type);
    }

    /**
     * 安排一个特定的变量绑定到一个查询结果集中给定的列。每次调用 PDOStatement::fetch()
     *  或 PDOStatement::fetchAll() 都将更新所有绑定到列的变量
     * <note>
     *      ①在语句执行前 PDO 有关列的信息并非总是可用，可移植的应用应在 PDOStatement::execute() 之后 调用此函数（方法）。
     *      ②但是，当使用 PgSQL 驱动 时，要想能绑定一个 LOB 列作为流，应用程序必须在调用 PDOStatement::execute() 之前调用此方法，
     *        否则大对象 OID 作为一个整数返回
     *      ③用法实例：
     *          $stmt = $dbh->prepare('SELECT name, colour, calories FROM fruit');
     *          $stmt->execute();//在execute之后、fetch之前调用
     *          $stmt->bindColumn(1, $name);
     *          $stmt->bindColumn(2, $colour);
     *          $stmt->bindColumn('calories', $cals);//通过名称绑定
     *          while ($row = $stmt->fetch(PDO::FETCH_BOUND)) {//参数传入PDO::FETCH_BOUND
     *              echo $name . "\t" . $colour . "\t" . $cals . "\n";
     *          }
     * </note>
     * @param int|string $column 结果集中的列号（从1开始索引）或列名。如果使用列名，注意名称应该与由驱动返回的列名大小写保持一致。
     * @param mixed $param 将绑定到列的 PHP 变量的引用
     * @param int  $type 通过 PDO::PARAM_* 常量指定的参数的数据类型
     * @param int  $maxlen 预分配提示
     * @param mixed $driverdata 驱动的可选参数
     * @return bool 成功时返回 TRUE， 或者在失败时返回 FALSE
     */
    public function bindColumn($column, &$param, $type = null, $maxlen = null, $driverdata = null){
        return $this->curStatement->bindColumn($column,$param,$type,$maxlen,$driverdata);
    }

    /**
     * 返回由 PDOStatement 对象代表的结果集中的列数
     * <note>
     *      ①只有在执行PDOStatement::execute()之后才能准确地获取列数，空的结果集的列数位0
     * </note>
     * @return int
     */
    public function columnCount(){
        return $this->curStatement->columnCount();
    }

    /**
     * 从结果集中获取下一行
     * @param int $fetch_style
     *              \PDO::FETCH_ASSOC 关联数组
     *              \PDO::FETCH_BOUND 使用PDOStatement::bindColumn()方法时绑定变量
     *              \PDO::FETCH_CLASS 放回该类的新实例，映射结果集中的列名到类中对应的属性名
     *              \PDO::FETCH_OBJ   返回一个属性名对应结果集列名的匿名对象
     * @param int $cursor_orientation 默认使用\PDO::FETCH_ORI_NEXT，还可以是PDO::CURSOR_SCROLL，PDO::FETCH_ORI_ABS，PDO::FETCH_ORI_REL
     * @param int $cursor_offset
     *              参数二设置为PDO::FETCH_ORI_ABS(absolute)时，此值指定结果集中想要获取行的绝对行号
     *              参数二设置为PDO::FETCH_ORI_REL(relative) 时 此值指定想要获取行相对于调用 PDOStatement::fetch() 前游标的位置
     * @return mixed 此函数（方法）成功时返回的值依赖于提取类型。在所有情况下，失败都返回 FALSE
     */
    public function fetch($fetch_style = null, $cursor_orientation = \PDO::FETCH_ORI_NEXT, $cursor_offset = 0){
        //设置fetch_style，未设置时使用默认
        isset($fetchStyle) or $fetch_style = self::$convention['PDO_DEFAULT_OPTION'][\PDO::ATTR_DEFAULT_FETCH_MODE];
        return $this->curStatement->fetch($fetch_style,$cursor_orientation,$cursor_offset);
    }

    /**
     * 返回一个包含结果集中所有剩余行的数组
     * 此数组的每一行要么是一个列值的数组，要么是属性对应每个列名的一个对象
     * @param int|null $fetch_style
     *          想要返回一个包含结果集中单独一列所有值的数组，需要指定 PDO::FETCH_COLUMN ，
     *          通过指定 column-index 参数获取想要的列。
     *          想要获取结果集中单独一列的唯一值，需要将 PDO::FETCH_COLUMN 和 PDO::FETCH_UNIQUE 按位或。
     *          想要返回一个根据指定列把值分组后的关联数组，需要将 PDO::FETCH_COLUMN 和 PDO::FETCH_GROUP 按位或
     * @param int $fetch_argument
     *                  参数一为PDO::FETCH_COLUMN时，返回指定以0开始索引的列（组合形式如上）
     *                  参数一为PDO::FETCH_CLASS时，返回指定类的实例，映射每行的列到类中对应的属性名
     *                  参数一为PDO::FETCH_FUNC时，将每行的列作为参数传递给指定的函数，并返回调用函数后的结果
     * @param array $constructor_args 参数二为PDO::FETCH_CLASS时，类的构造参数
     * @return array
     */
    public function fetchAll($fetch_style = null, $fetch_argument = null, $constructor_args = null){
        $param = array();
        isset($fetch_style)         and $param[0] = $fetch_style;
        isset($fetch_argument)      and $param[1] = $fetch_argument;
        isset($constructor_args)    and $param[2] = $constructor_args;
        return call_user_func_array(array($this->curStatement,'fetchAll'),$param);
    }

    /**
     * 从结果集中的下一行返回单独的一列。
     * （这样的一列返回后，结果集中的指针将往后移动）
     * <note>
     *      ①这个方法很有用处的是：(直接获取记录数目)
     *          $db = new PDO('mysql:host=localhost;dbname=pictures','user','password');
     *          $pics = $db->query('SELECT COUNT(id) FROM pics');
     *          $this->totalpics = $pics->fetchColumn();
     *          $db = null; // 释放PDO等对象使其等待回收
     * </note>
     * @param int $column_number 列的索引，默认是第一列
     * @return string 从结果集中的下一行返回单独的一列，如果没有了，则返回 FALSE
     */
    public function fetchColumn($column_number = 0){
        return $this->curStatement->fetchColumn($column_number);
    }

    /**
     * 获取下一行并作为一个对象返回
     * 适合做框架中的Model类
     * 说明：获取下一行并作为一个对象返回。此函数（方法）是使用 PDO::FETCH_CLASS 或 PDO::FETCH_OBJ 风格的 PDOStatement::fetch() 的一种替代
     * @param string $class_name 类的名称,默认是stdClass类
     * @param array $constructor_args 构造函数参数
     * @return bool|Object 返回一个属性名对应于列名的所要求类的实例， 或者在失败时返回 FALSE
     */
    public function fetchObject($class_name = 'stdClass', array $constructor_args = []){
        return $this->curStatement->fetchObject($class_name,$constructor_args);
    }

    /**
     * 返回上一个由对应的 PDOStatement 对象执行DELETE、 INSERT、或 UPDATE 语句受影响的行数
     * 如果上一条由相关 PDOStatement 执行的 SQL 语句是一条 SELECT 语句，有些数据可能返回由此语句返回的行数
     * 但这种方式不能保证对所有数据有效，且对于可移植的应用不应依赖于此方式
     * @return int
     * @throws CoraxException
     */
    public function rowCount(){
        if(!$this->curStatement){
            throw new CoraxException('Invalid PDOStatement');
        }
        return $this->curStatement->rowCount();
    }


    /*TODO:错误信息获取 ***************************************************************************************/

    /**
     * 返回PDO驱动或者上一个PDO语句对象上发生的错误的信息（具体驱动的错误号和错误信息）
     * @return string 返回错误信息字符串，没有错误发生时返回空字符串
     */
    public function getErrorInfo(){
        return $this->error;
    }
    /**
     * 获取PDO对象查询时发生的错误
     * @return string
     */
    public function getPdoErrorInfo(){
        $pdoError = $this->driver->errorInfo();
        return isset($pdoError[1])?"Code:{$pdoError[0]} [{$pdoError[1]}]:[{$pdoError[2]}]":'';
    }
    /**
     * 获取PDOStatemnent对象上查询时发生的错误
     * @return string
     */
    public function getStatementErrorInfo(){
        $stmtError = $this->curStatement->errorInfo();
        return isset($stmtError[1])?"[{$stmtError[1]}]:[{$stmtError[2]}]":'';
    }



    /*TODO:事务功能 ***************************************************************************************/

    /**
     * 开启事务
     * @return bool
     */
    public function beginTransaction(){
        return $this->driver->beginTransaction();
    }

    /**
     * 提交事务
     * @return bool
     */
    public function commit(){
        return $this->driver->commit();
    }
    /**
     * 回滚事务
     * @return bool
     */
    public function rollBack(){
        return $this->driver->rollBack();
    }
    /**
     * 确认是否在事务中
     * @return bool
     */
    public function inTransaction(){
        return $this->driver->inTransaction();
    }

    /**
     * 释放到数据库服务的连接，以便发出其他 SQL 语句(新的参数绑定)，使得该SQL语句处于一个可以被再次执行的状态
     * 当上一个执行的 PDOStatement 对象仍有未取行时，此方法对那些不支持再执行一个 PDOStatement 对象的数据库驱动非常有用。
     * 如果数据库驱动受此限制，则可能出现失序错误的问题
     * PDOStatement::Cursor() 要么是一个可选驱动的特有方法（效率最高）来实现，要么是在没有驱动特定的功能时作为一般的PDO 备用来实现
     * <note>
     *      ① 语意上相当于下面的语句的执行结果
     *          do {
     *              while ($stmt->fetch());
     *              if (!$stmt->nextRowset()) break;
     *          } while (true);
     * </note>
     * @param \PDOStatement|null $statement
     * @return bool 成功时返回 TRUE， 或者在失败时返回 FALSE
     */
    public function closeCursor($statement=null){
        isset($statement) and $this->curStatement = $statement;
        return $this->curStatement->closeCursor();
    }


    /**
     * 获取预处理语句包含的信息
     * <note>
     *      ①实际不能获取参数的值，不像文档中写的那样
     *      ②无论是否发生了错误，信息都会存在
     * </note>
     * @return string
     */
    public function getStatementParams(){
        ob_start();//开始本层次的ob缓冲区
        $this->curStatement->debugDumpParams();
        return ob_get_clean();// 相当于ob_get_contents() 和 ob_end_clean()
    }



    /*TODO:扩展方法 ******************************************************************************************/
    /**
     * 添加数据
     * <code>
     *      $fldsMap ==> array(
     *          'fieldName' => 'fieldValue',
     *          'fieldName' => array('fieldValue',boolean),//第二个元素表示是否对字段名称进行转义
     *      );
     *
     *     $data = ['a'=>'foo','b'=>'bar'];
     *     $keys = array_keys($data);
     *     $fields = '`'.implode('`, `',$keys).'`';
     *     #here is my way
     *     $placeholder = substr(str_repeat('?,',count($keys),0,-1));
     *     $pdo->prepare("INSERT INTO `baz`($fields) VALUES($placeholder)")->execute(array_values($data));
     * </code>
     *
     * 插入数据的sql可以是：
     * ①INSERT INTO 表名称 VALUES (值1, 值2,....)
     * ②INSERT INTO table_name (列1, 列2,...) VALUES (值1, 值2,....)
     *
     * @param string $tablename
     * @param array $fieldsMap
     * @return bool 返回true或者false
     * @throws CoraxException
     */
    public function create($tablename,$fieldsMap){
        $fields = $placeholder = '';
        $sql = null;
        $bind  = [];
        $flag = true;//标记是否进行插入形式判断

        foreach($fieldsMap as $fieldName=>$fieldValue){
            $colnm = $fieldName;
            if($flag){
                if(is_numeric($fieldName)){
                    $placeholder  = rtrim(str_repeat(' ?,',count($fieldsMap)),',');
                    $sql = "INSERT INTO {$tablename} VALUES ( {$placeholder} );";
                    $bind = $fieldsMap;
                    break;
                }
                $flag = false;
            }
            if(is_array($fieldValue)){ //不设置字段名称进行插入时$fieldName无意义
                $colnm = $fieldValue[1]?$this->driver->escapeField($fieldName):$fieldName;
                $fieldValue = $fieldValue[0];
            }
            $fields .= " {$colnm} ,";
            $placeholder  .= " :{$fieldName} ,";
            $bind[":{$fieldName}"] = $fieldValue;
        }

        if(isset($sql)){
            $fields = rtrim($fields,',');
            $sql = "INSERT INTO {$tablename} ( {$fields} ) VALUES ( {$placeholder} );";
        }
        return $this->prepare($sql)->execute($bind);
    }

    /**
     * 更新数据表
     * @param string $tablename
     * @param string|array $flds
     * @param string|array $whr
     * @return bool
     * @throws CoraxException
     */
    public function update($tablename,$flds,$whr){;
        $input_params = [];
        $fields = is_string($flds)?[$flds,[]]:$this->makeSegments($flds,false);
        $where  = is_string($whr) ?[$whr,[]] :$this->makeSegments($whr, false);
        empty($fields[1]) or $input_params = $fields[1];
        empty($where[1]) or array_merge($input_params,$where[1]);
        return $this->prepare("UPDATE {$tablename} SET {$fields[0]} WHERE {$where[0]};")->execute($input_params);
    }

    /**
     * 执行删除数据的操作
     * 如果不设置参数，则进行清空表的操作
     * @param string $tablename 数据表的名称
     * @param array $whr 字段映射数组
     * @return bool
     */
    public function delete($tablename,$whr=null){
        $bind = null;
        if(isset($whr)){
            $where  = $this->makeSegments($whr);
            $sql    = "delete from {$tablename} where {$where[0]};";
            $bind   = $where[1];
        }else{
            $sql = "delete from {$tablename};";
        }
        return $this->prepare($sql)->execute($bind);
    }

    /**
     * ��ѯһ��SQL
     * @param string $tablename
     * @param string|array|null $fields
     * @param string|array|null $whr
     * @return array|bool
     * @throws CoraxException
     */
    public function select($tablename,$fields=null,$whr=null){
        $bind = null;

        //设置选取字段
        if(null === $fields){
            $fields = ' * ';
        }elseif($fields and is_array($fields)){
            //默认转义
            array_map(function($param){
                return $this->driver->escapeField($param);
            },$fields);
            $fields = implode(',',$fields);
        }elseif(!is_string($fields)){
            throw new CoraxException('Parameter 2 require the type of "null","array","string" ,now is invalid!');
        }

        if(null === $whr){
            $sql = "select {$fields} from {$tablename};";
        }elseif(is_array($whr)){
            $whr  = is_string($whr)? [$whr,null] :$this->makeSegments($whr);
            $sql = "select {$fields} from {$tablename} where {$whr[0]};";
            $bind = $whr[1];
        }elseif(is_string($whr)){
            $sql = "select {$fields} from {$tablename} where {$whr};";
        }else{
            throw new CoraxException('Parameter 3 require the type of "null","array","string" ,now is invalid!');
        }


        if(false === $this->prepare($sql)->execute($bind) ){
            return false;
        }
        return $this->fetchAll();
    }





    /**
     * 综合字段绑定的方法
     * <code>
     *      $operator = '='
     *          $fieldName = :$fieldName
     *          :$fieldName => trim($fieldValue)
     *
     *      $operator = 'like'
     *          $fieldName = :$fieldName
     *          :$fieldName => dowithbinstr($fieldValue)
     *
     *      $operator = 'in|not_in'
     *          $fieldName in|not_in array(...explode(...,$fieldValue)...)
     * </code>
     * @param string $fieldName 字段名称
     * @param string|array $fieldValue 字段值
     * @param string $operator 操作符
     * @param bool $translate 是否对字段名称进行转义,MSSQL中使用[]
     * @return array
     * @throws CoraxException
     */
    protected function makeFieldBind($fieldName,$fieldValue,$operator='=',$translate=false){
        $fieldName = trim($fieldName,' :[]');
        $bindFieldName = null;
        if(false !== strpos($fieldName,'.')){
            $arr = explode('.',$fieldName);
            $bindFieldName = ':'.array_pop($arr);
        }elseif(mb_strlen($fieldName,'utf-8') < strlen($fieldName)){//其他编码
            $bindFieldName = ':'.md5($fieldName);
        }else{
            $bindFieldName = ":{$fieldName}";
        }

        $operator = strtolower(trim($operator));
        $sql = $translate?" [{$fieldName}] ":" {$fieldName} ";
        $bind = array();

        switch($operator){
            case '=':
                $sql .= " = {$bindFieldName} ";
                $bind[$bindFieldName] = $fieldValue;
                break;
            case 'like':
                $sql .= " like {$bindFieldName} ";
                $bind[$bindFieldName] = $fieldValue;
                break;
            case 'in':
            case 'not in':
                if(is_string($fieldValue)){
                    $sql .= " {$operator} ({$fieldValue}) ";
                }elseif(is_array($fieldValue)){
                    $sql .= " {$operator} ('".implode("','",$fieldValue)."')";
                }else{
                    throw new CoraxException("The parameter 1 '{$fieldValue}' is invalid!");
                }
                break;
            default:
                throw new CoraxException("The parameter 2 '{$operator}' is invalid!");
        }
        return [$sql,$bind];
    }

    /**
     * 片段设置
     * <note>
     *      片段准则
     *      $map == array(
     *           //第一种情况,连接符号一定是'='//
     *          'key' => $val,
     *          'key' => array($val,$operator,true),
     *          //第二种情况，数组键，数组值//
     *          array('key','val','like|=',true),//参数4的值为true时表示对key进行[]转义
     *          //第三种情况，字符键，数组值//
     *          'assignSql' => array(':bindSQLSegment',value)//与第一种情况第二子目相区分的是参数一以':' 开头
     *      );
     * </note>
     * @param $map
     * @param string $connect 表示是否使用and作为连接符，false时为,
     * @return array
     */
    public function makeSegments($map,$connect='and'){
        //初始值与参数检测
        $bind = [];
        $sql = '';
        if(empty($map)){
            return [$sql,$bind];
        }

        //元素连接
        foreach($map as $key=>$val){
            if(is_numeric($key)){
                //第二种情况
                $rst = $this->makeFieldBind(
                    $val[0],
                    $val[1],
                    isset($val[2])?$val[2]:' = ',
                    !empty($val[3])
                );
                if(is_array($rst)){
                    $sql .= " {$rst[0]} {$connect}";
                    $bind = array_merge($bind, $rst[1]);
                }
            }elseif(is_array($val) and strpos($val[0],':') === 0){
                //第三种情况,复杂类型，由用户自定义
                $sql .= " {$key} {$connect}";
                $bind[$val[0]] = $val[1];
            }else{
                //第一种情况
                $translate = false;
                $operator = '=';
                if(is_array($val)){
                    $translate = isset($val[2])?$val[2]:false;
                    $operator = isset($val[1])?$val[1]:'=';
                    $val = $val[0];
                }
                $rst = $this->makeFieldBind($key,trim($val),$operator,$translate);//第一种情况一定是'='的情况
                if(is_array($rst)){
                    $sql .= " {$rst[0]} {$connect}";
                    $bind = array_merge($bind, $rst[1]);
                }
            }
        }
        $result = array(
            substr($sql,0,strlen($sql)-strlen($connect)),//去除最后一个and
            $bind,
        );
        return $result;
    }


    /**
     * 编译组件成适应当前数据库的SQL字符串
     * @param array $components  复杂SQL的组成部分
     * @return string
     */
    public function compile($components){
        return $this->driver->compile($components);
    }


    /**
     * 获取数据表字段
     * @access public
     * @param $tableName
     * @return array
     */
    public function getFields($tableName){
        return $this->driver->getFields($tableName);
    }

    /**
     * 转义保留字字段名称
     * @param string $fieldname 字段名称
     * @return string
     */
    public function escape($fieldname){
        return $this->driver->escapeField($fieldname);
    }

    /**
     * 执行结果信息返回
     * @return int|string 返回受影响行数，发生错误时返回错误信息
     */
    public function doneExecute(){
        if(null === $this->error){
            //未发生错误，返回受影响的行数目
            return $this->rowCount();
        }else{
            //发生饿了错误，得到错误信息并清空错误标记
            $temp = $this->error;
            $this->error = null;
            return $temp;
        }
    }
    /**
     * 查询结果集全部返回
     * 内部实现依赖于fetchAll方法，参数同
     * @param null $fetch_style
     * @param null $fetch_argument
     * @param null $constructor_args
     * @return string|Dao 返回查询结果集，发生错误时返回错误信息
     */
    public function doneQuery($fetch_style = null, $fetch_argument = null, $constructor_args = null){
        if(null === $this->error){
            //未发生错误，返回受影响的行数目
            return $this->fetchAll($fetch_style, $fetch_argument, $constructor_args);
        }else{
            //发生饿了错误，得到错误信息并清空错误标记
            $temp = $this->error;
            $this->error = null;
            return $temp;
        }
    }


}
