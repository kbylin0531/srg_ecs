<?php
/**
 * Created by PhpStorm.
 * User: Lin
 * Date: 2015/11/10
 * Time: 20:37
 */
namespace System\Core\Router;
use System\Core\Router;
use System\Core\Unique;
use System\Exception\CoraxException;
use System\Util\UDK;

/**
 * Class RuleCracker 规则解析
 * @package System\Core\Router
 */
class RuleParser extends Unique {

    /**
     * 惯例配置配置
     * @var array
     */
    protected $convention = [

        //直接路由发生在URL解析之前，直接路由如果匹配了URL字符串，则直接链接到指定的模块，否则将进行URL解析和间接路由
        'DIRECT_ROUTE_RULES'    => [
            //静态路由规则
            'DIRECT_STATIC_ROUTE_RULES' => [],
            //通配符路由规则,具体参考CodeIgniter
            'DIRECT_WILDCARD_ROUTE_RULES' => [],
            //正则表达式 规则
            'DIRECT_REGULAR_ROUTE_RULES' => [],
        ],
        //间接路由在URL解析之后
        'INDIRECT_ROUTE_RULES'   => [],
    ];
    /**
     * 解析过的缓存
     * @var array
     */
    private $cache = [];

    /**
     * 解析简介路由规则，即URL解析过后的路由规则再解析
     * @param string|array $msnm 模块序列
     * @param string $ctlnm 控制器名称
     * @param string $actnm 操作名称
     * @param array|string $params 参数列表
     * @return array|null 解析结果数组，null时表示未匹配
     */
    public function parseIndirectRules($msnm,$ctlnm,$actnm,$params=''){
        if(isset($this->convention['INDIRECT_ROUTE_RULES'])){
            foreach($this->convention['DIRECT_ROUTE_RULES'] as $rule => $target){
                $key = Router::buildKey($msnm,$ctlnm,$actnm,$params);
                if(0 === strcasecmp($key,$rule)){
                    if(is_callable($target)){
                        $target = call_user_func($target);
                        if(!is_string($target) and !is_array($target)){
                            //要求结果必须返回string或者数组
                            return null;
                        }
                    }
                    return isset($target)?[
                        'm' => isset($target[0])?$target[0]:null,
                        'c' => isset($target[1])?$target[1]:null,
                        'a' => isset($target[2])?$target[2]:null,
                        'p' => isset($target[3])?$target[3]:null,
                    ]:null;
                }
            }
        }
        return null;
    }

    /**
     * 解析直接路由规则
     * 直接路由规则将分为三类：
     *  ①静态直接路由地址
     *  ②匹配符路由规则
     *  ③正则式路由规则
     * 优先级从高到低级排列
     * @param string $uri uri地址
     * @return array|null|string 返回array表示完整的解析结果
     *                           返回string将交给Router继续代替原始地址进行进一步的解析
     *                           返回null表示未找到匹配项目
     */
    public function parseDirectRules($uri){
        if(!isset($this->cache[$uri])){
            $target = $this->parseStatic($uri);
            if(null === $target){
                $target = $this->parseWildcard($uri);
                if(null === $target){
                    $target = $this->parseRegular($uri);
                }
            }
            $this->cache[$uri] = (isset($target) and is_array($target))?[
                'm' => isset($target[0])?$target[0]:null,
                'c' => isset($target[1])?$target[1]:null,
                'a' => isset($target[2])?$target[2]:null,
                'p' => isset($target[3])?$target[3]:null,
            ]:$target;
        }
        return $this->cache[$uri];
    }

    /**
     * 解析静太路由规则,解析时忽略大小写
     * 返回非null值时表示
     * @param string $url url地址
     * @return array|string|null
     * @throws CoraxException
     */
    protected function parseStatic($url){
        if(isset($this->convention['DIRECT_ROUTE_RULES']['DIRECT_STATIC_ROUTE_RULES'])){
            foreach($this->convention['DIRECT_ROUTE_RULES']['DIRECT_STATIC_ROUTE_RULES'] as $rule => $target){
                if(0 === strcasecmp($url,trim($rule))){
                    if(is_callable($target)){
                        $target = call_user_func_array($target,[$rule]);
                    }
                    if(is_string($target) or is_array($target)){
                        return $target;
                    }else{
                        //匹配了但是规则不符合，直接报错退出
                        throw new CoraxException('Unexpect parameter!'.var_export($target,true));
                    }
                }
            }
        }
        return null;
    }


    /**
     * 解析通配符路由规则
     * 实际上通过正则表达式简介实现
     * @param string $uri 待匹配的URI地址
     * @return array|string|null
     */
    protected function parseWildcard($uri){
        if(isset($this->convention['DIRECT_ROUTE_RULES']['DIRECT_WILDCARD_ROUTE_RULES'])){
            foreach($this->convention['DIRECT_ROUTE_RULES']['DIRECT_WILDCARD_ROUTE_RULES'] as $rule => $target){
                $rule = preg_replace('/\[.+?\]/','([^/\[\]]+)',$rule);//非贪婪匹配
                $rst = $this->_matchRegular($rule,$target, trim($uri,' /'));
                if(isset($rst)){
                    return $rst;
                }
            }
        }
        return null;
    }

    /**
     * 解析通配符正则表达式路由规则
     * @param string $uri 待匹配的URI地址
     * @return mixed|null
     */
    protected function parseRegular($uri){
        if(isset($this->convention['DIRECT_ROUTE_RULES']['DIRECT_REGULAR_ROUTE_RULES'])){
            foreach($this->convention['DIRECT_ROUTE_RULES']['DIRECT_REGULAR_ROUTE_RULES'] as $rule => $target){
                $rst = $this->_matchRegular($rule,$target, trim($uri,' /'));
                if(isset($rst)){
                    return $rst;
                }
            }
        }
        return null;
    }

    /**
     * 使用正则表达式匹配uri
     * @param string $rule 路由规则
     * @param array|string|callable $target 路由导向结果
     * @param string $uri 传递进来的URL字符串
     * @return array|string|null
     */
    private function _matchRegular($rule,$target, $uri){
        // Does the RegEx match? use '#' to ignore '/'
        if (preg_match('#^'.$rule.'$#', $uri, $matches)) {
            if(is_array($target)){
                if(isset($target[3])){
                    $index = 1;//忽略第一个匹配（全匹配）
                    foreach($target[3] as $pname=>&$pval){
                        if(isset($matches[$index])){
                            $pval = $matches[$index];
                        }
                        ++$index;
                    }
                    $_GET = array_merge($target[3],$_GET);// 优先使用$_GET覆盖
                }else{
                    //未设置参数项，不作动作
                }
            }elseif(is_string($target)){
                $target = preg_replace('#^'.$rule.'$#', $target, $uri);//参数一代表的正则表达式从参数三的字符串中寻找匹配并替换到参数二代表的字符串中
            }elseif(is_callable($target)){
                // Remove the original string from the matches array.
                $fulltext = array_shift($matches);
                // Execute the callback using the values in matches as its parameters.
                $target = call_user_func_array($target, [$matches,$fulltext]);//参数二是完整的匹配
                if(!is_string($target) and !is_array($target)){
                    //要求结果必须返回string或者数组
                    return null;
                }
            }
            return $target;
        }
        return null;
    }


}