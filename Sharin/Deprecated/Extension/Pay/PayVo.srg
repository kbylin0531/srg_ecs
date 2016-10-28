<?php
namespace System\Extension\Pay;

/**
 * 订单数据模型
 */
class PayVo {

    protected $_orderNo;
    protected $_fee;
    protected $_title;
    protected $_body;
    protected $_callback;
    protected $_url;
    protected $_param;

    /**
     * 设置订单号
     * @param mixed $order_no
     * @return PayVo
     */
    public function setOrderNo($order_no) {
        $this->_orderNo = $order_no;
        return $this;
    }

    /**
     * 设置商品价格
     * @param mixed $fee
     * @return PayVo
     */
    public function setFee($fee) {
        $this->_fee = $fee;
        return $this;
    }

    /**
     * 设置商品名称
     * @param mixed $title
     * @return PayVo
     */
    public function setTitle($title) {
        $this->_title = $title;
        return $this;
    }

    /**
     * 设置商品描述
     * @param mixed $body
     * @return PayVo
     */
    public function setBody($body) {
        $this->_body = $body;
        return $this;
    }

    /**
     * 设置支付完成后的后续操作接口
     * @param mixed $callback
     * @return PayVo
     */
    public function setCallback($callback) {
        $this->_callback = $callback;
        return $this;
    }

    /**
     * 设置支付完成后的跳转地址
     * @param mixed $url
     * @return PayVo
     */
    public function setUrl($url) {
        $this->_url = $url;
        return $this;
    }

    /**
     * 设置订单的额外参数
     * @param mixed $param
     * @return PayVo
     */
    public function setParam($param) {
        $this->_param = $param;
        return $this;
    }

    /**
     * 获取订单号
     * @return mixed
     */
    public function getOrderNo() {
        return $this->_orderNo;
    }

    /**
     * 获取商品价格
     * @return mixed
     */
    public function getFee() {
        return $this->_fee;
    }

    /**
     * 获取商品名称
     * @return mixed
     */
    public function getTitle() {
        return $this->_title;
    }

    /**
     * 获取支付完成后的后续操作接口
     * @return mixed
     */
    public function getCallback() {
        return $this->_callback;
    }

    /**
     * 获取支付完成后的跳转地址
     * @return mixed
     */
    public function getUrl() {
        return $this->_url;
    }

    /**
     * 获取商品描述
     * @return mixed
     */
    public function getBody() {
        return $this->_body;
    }

    /**
     * 获取订单的额外参数
     * @return mixed
     */
    public function getParam() {
        return $this->_param;
    }

}
