<?php
/**
 * Created by PhpStorm.
 * User: Lin
 * Date: 2015/11/8
 * Time: 17:30
 */
namespace System\Core\Router;
use System\Core\Router;
use System\Core\Unique;
use System\Exception\CoraxException;
use System\Util\SEK;

/**
 * Class DomainCreater 域名创建器
 * 根据模块创建可以访问的域名
 * @package System\Core\Router
 */
class DomainCreater extends Unique{
    /**
     * 惯例配置
     * @var array
     */
    protected $convention = [
        //子域名部署模式下 的 完整域名
        'FUL_DOMAIN'    => '',
        //是否将子域名和模块进行对应
        'SUB_DOMAIN_MODULE_MAPPING_ON'  => true,
        //子域名部署规则
        'SUB_DOMAIN_DEPLOY_RULES'   => [],
    ];
    /**
     * 模块缓存
     * @var array
     */
    protected $cache = [];

    /**
     * 初始化构造
     * @param array|null $config
     * @throws CoraxException
     * @throws \System\Exception\ParameterInvalidException
     */
    public function __construct(array $config=null){
        parent::__construct($config);
        $this->buildCache();
    }

    /**
     * 创建域名映射缓存
     * @param array|null $rules
     * @return array
     * @throws CoraxException
     */
    protected function buildCache(array $rules=null){
        //TODO:缓存检查,存在则直接返回缓存中的数据
        isset($rules) or $rules = $this->convention['SUB_DOMAIN_DEPLOY_RULES'];
        if(!$rules) return [];
        foreach($rules as $subdomain=>$modulerule){
            if(is_string($modulerule)){
                if(SEK::isRedirectLink($modulerule)){
                    //如果是跳转，则不建立映射关系
                    continue;
                }
            }elseif(is_array($modulerule)){
                $modulerule = Router::toModulesString($modulerule[0],'/');
            }else{
                throw new CoraxException('错误的路由规则',$modulerule);
            }
            $this->cache[strtolower($modulerule)] = $subdomain;
        }
//        UDK::dump($this->moduleCache);
        return $this->cache;
    }

    /**
     *
     * @param string|array $modules 控制器序列
     * @return string|null 返回完整的域名或者null(表示无匹配)
     * @throws CoraxException
     */
    public function create($modules){
        if(!$this->cache) return null;

        $hostname = null;
        $key = strtolower(Router::toModulesString($modules));//装换成小写序列
        if(isset($this->cache[$key])){
            //看是否设置了部署
            $hostname = $this->cache[$key].'.'.$this->convention['FUL_DOMAIN'];
        }elseif($this->convention['SUB_DOMAIN_MODULE_MAPPING_ON']){
            //自动映射
            if(is_string($modules)){
                if(false !== strpos($modules,'/')){
                    $modules = str_replace('/','.',$modules);
                }
            }elseif(is_array($modules)){
                $modules = implode('.',$modules);
            }else{
                throw new CoraxException('Invalid Parameter!');
            }
            $hostname = strtolower($modules).'.'.$this->convention['FUL_DOMAIN'];
        }
        return $hostname;
    }

}