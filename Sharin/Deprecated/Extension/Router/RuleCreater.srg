<?php
/**
 * Created by PhpStorm.
 * User: Lin
 * Date: 2015/11/10
 * Time: 20:35
 */
namespace System\Core\Router;
use System\Core\Router;
use System\Core\Unique;
use System\Util\SEK;

/**
 * Class RuleCreater 应用路由规则来创建URL
 * 简介路由的定位是跳转，故不作路由规则创建
 * @package System\Core\Router
 */
class RuleCreater extends Unique {

    private $ruleCache = null;

    /**
     * 构造函数
     * @param array|null $config
     */
    protected function __construct(array $config=null){
        parent::__construct($config);
        $this->buildCache();
    }

    protected function buildCache(){
        //直接静态路由缓存
        foreach($this->convention['DIRECT_ROUTE_RULES']['DIRECT_STATIC_ROUTE_RULES'] as $rule=>$conf){
            if(is_string($conf)){
                if(!SEK::isRedirectLink($conf)){

                }else{
                    //直接跳转链接,不缓存
                }
            }elseif(is_array($conf)){

            }else{
                //不作限定
            }

        }
        //直接通配路由缓存
        //直接正则路由缓存(忽略)
    }

    /**
     * URL规则地址生成器
     * 由于访问框架下的资源必须要精确到指定的action，所以前三个参数是必须要指定的
     * @param string|array $modulesback
     * @param string $ctlname
     * @param string $actname
     * @param array|null $params
     * @param array|null $modules
     */
    public function create($modulesback=null,$ctlname=null,$actname=null,$params=null,$modules=null){


    }



}