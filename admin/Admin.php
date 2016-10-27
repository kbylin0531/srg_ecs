<?php
define('IN_ECS', true);

require(__DIR__ . '/includes/init.php');
/**
 * Class Admin
 */
class Admin {

    protected $ecs = null;

    protected $db = null;

    protected $smarty  = null;

    protected $_LANG = null;

    protected $exc_user = null;

    protected $exc = null;

    public function __construct() {
        global $ecs,$db,$smarty,$_LANG,$exc ,$exc_user;
        $this->ecs = $ecs;
        $this->db = $db;
        $this->smarty = $smarty ;
        $this->_LANG = $_LANG;
        $this->exc = $exc;
        $this->exc_user = $exc_user;
    }
}