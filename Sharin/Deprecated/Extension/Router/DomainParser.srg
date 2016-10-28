<?php
/**
 * Created by PhpStorm.
 * User: Lin
 * Date: 2015/11/8
 * Time: 16:44
 */
namespace System\Core\Router;
use System\Core\Router;
use System\Core\Unique;
use System\Exception\CoraxException;
use System\Util\SEK;

/**
 * Class DomainParser 域名解析类
 * @package System\Core\Router
 */
class DomainParser extends Unique{
    /**
     * 惯例配置
     * @var array
     */
    protected $convention = [
        //子域名部署模式下 的 完整域名
        'FUL_DOMAIN'=>'',
        //使用的协议名称
        'HOST_PROTOCOL' => 'http',
        //使用的端口号，默认为80时会显示为隐藏
        'HOST_PORT' => 80,
        //是否将子域名和模块进行对应
        'SUB_DOMAIN_MODULE_MAPPING_ON'  => true,
        //子域名部署规则
        'SUB_DOMAIN_DEPLOY_RULES' => [
            /**
             * 分别对应子域名模式下 的 [模块、(控制器、(操作、(参数)))]
             * 控制器到参数为可选单元，模块对应着子域名
             * 设置为null是表示不做设置，将使用默认的通用配置
             *
             * 部署规则的反面则对应着 "模块序列"=>"子域名首部" 的键值对
             */
        ],
    ];

    /**
     * 解析结果，未解析出来的部分值为null，表示不覆盖原有的结果
     * @var array
     */
    protected $result = null;

    /**
     * 解析域名
     * @param string|null $hostname 主机名称,当设置为null，或者未设置参数时自动取自$_SERVER['HTTP_HOST']
     * @return array|string|null 域名对应的模块、控制器、操作、参数等信息,如果返回string则表示重定向地址,返回null表示未匹配任何子域名
     * @throws CoraxException
     */
    public function parse($hostname=null){
        static $cache = [];
        //预处理
        isset($hostname) or $hostname = strtolower($_SERVER['HTTP_HOST']);

        if(!isset($cache[$hostname])){
            $cache[$hostname] = [];
            //获取子域名的部分，并且转小写
            $subname = strtolower(trim(strstr($hostname, $this->convention['FUL_DOMAIN'], true), '. '));
            if(empty($subname)){
                return $cache[$hostname] = null;
            }
            //检查是否存在子域名对应的规则
            if ($subname and isset($this->convention['SUB_DOMAIN_DEPLOY_RULES'][$subname])){
                //获取对应的规则并解析之
                $subdomain_rule = &$this->convention['SUB_DOMAIN_DEPLOY_RULES'][$subname];
                //分析
                if(is_string($subdomain_rule)){
                    if(SEK::isRedirectLink($subdomain_rule)){
                        return $subdomain_rule;// 重定向地址
                    }else{
                        //认为只是模块序列
                        $cache[$hostname]['m'] = Router::toModulesArray($subdomain_rule);
                    }
                }elseif(is_array($subdomain_rule)){
                    //获取modules(模块数组)
                    if (isset($subdomain_rule[0])) { // 只允许string或者array类型的参数
                        $cache[$hostname]['m'] = Router::toModulesArray($subdomain_rule[0]);
                    }
                    //获取controller(控制器字符串)
                    $cache[$hostname]['c'] = isset($subdomain_rule[1]) ? ucfirst($subdomain_rule[1]) : NULL;
                    //获取action(操作字符串) 注意的是控制器中方法不区分大小写
                    $cache[$hostname]['a'] = isset($subdomain_rule[2]) ? $subdomain_rule[2] : NULL;
                    //获取parameters(额外参数)
                    if (isset($subdomain_rule[3])) {
                        $query = null;
                        if(is_array($subdomain_rule[3])){
                            $cache[$hostname]['p'] = $subdomain_rule[3];
                        }elseif(is_string($subdomain_rule[3])){
                            parse_str($subdomain_rule[3], $cache[$hostname]['p']);
                        }else{
                            throw new CoraxException($subdomain_rule[3]);
                        }
                    }
                }else{
                    throw new CoraxException($subdomain_rule);
                }
            }elseif($this->convention['SUB_DOMAIN_MODULE_MAPPING_ON']){
                //检查是否进行了模块域名映射
                if(false !== strpos($subname,'.')){
                    $cache[$hostname]['m'] = Router::toModulesArray($subname,'.');
                }else{
                    $cache[$hostname]['m'] = ucfirst($subname);
                }
            }
        }

        //如果不是空数组，返回的结果即解析结果，否则返回null表示完全未匹配
        if($cache[$hostname]) $this->result = $cache[$hostname];
        return $this->result;
    }



}