<?php

/**
 * ECSHOP 会员等级管理程序
 * ============================================================================
 * * 版权所有 2005-2012 上海商派网络科技有限公司，并保留所有权利。
 * 网站地址: http://www.ecshop.com；
 * ----------------------------------------------------------------------------
 * 这不是一个自由软件！您只能在不用于商业目的的前提下对程序代码进行修改和
 * 使用；不允许对程序代码以任何形式任何目的的再发布。
 * ============================================================================
 * $Author: liubo $
 * $Id: user_rank.php 17217 2011-01-19 06:29:08Z liubo $
*/

define('IN_ECS', true);

class UserRank extends Admin {

    /*------------------------------------------------------ */
//-- 会员等级列表
    /*------------------------------------------------------ */

    public function lists ()
    {
        $ranks = $this->db ->getAll("SELECT * FROM " .$this->ecs->table('user_rank'));

        $this->smarty ->assign('ur_here',      $this->_LANG['05_user_rank_list']);
        $this->smarty ->assign('action_link',  array('text' => $this->_LANG['add_user_rank'], 'href'=>'user_rank.php?act=add'));
        $this->smarty ->assign('full_page',    1);

        $this->smarty ->assign('user_ranks',   $ranks);

        assign_query_info();
        $this->smarty ->display('user_rank.htm');
    }

    /*------------------------------------------------------ */
//-- 翻页，排序
    /*------------------------------------------------------ */
    public function query ()
    {
        $ranks = $this->db ->getAll("SELECT * FROM " .$this->ecs->table('user_rank'));

        $this->smarty ->assign('user_ranks',   $ranks);
        make_json_result($this->smarty ->fetch('user_rank.htm'));
    }

    /*------------------------------------------------------ */
//-- 添加会员等级
    /*------------------------------------------------------ */

    public function add ()
    {
        admin_priv('user_rank');

        $rank['rank_id']      = 0;
        $rank['rank_special'] = 0;
        $rank['show_price']   = 1;
        $rank['min_points']   = 0;
        $rank['max_points']   = 0;
        $rank['discount']     = 100;

        $form_action          = 'insert';

        $this->smarty ->assign('rank',        $rank);
        $this->smarty ->assign('ur_here',     $this->_LANG['add_user_rank']);
        $this->smarty ->assign('action_link', array('text' => $this->_LANG['05_user_rank_list'], 'href'=>'user_rank.php?act=list'));
        $this->smarty ->assign('ur_here',     $this->_LANG['add_user_rank']);
        $this->smarty ->assign('form_action', $form_action);

        assign_query_info();
        $this->smarty ->display('user_rank_info.htm');
    }

    /*------------------------------------------------------ */
//-- 增加会员等级到数据库
    /*------------------------------------------------------ */

    public function insert ()
    {
        admin_priv('user_rank');

        $special_rank =  isset($_POST['special_rank']) ? intval($_POST['special_rank']) : 0;
        $_POST['min_points'] = empty($_POST['min_points']) ? 0 : intval($_POST['min_points']);
        $_POST['max_points'] = empty($_POST['max_points']) ? 0 : intval($_POST['max_points']);

        /* 检查是否存在重名的会员等级 */
        if (!$this->exc->is_only('rank_name', trim($_POST['rank_name'])))
        {
            sys_msg(sprintf($this->_LANG['rank_name_exists'], trim($_POST['rank_name'])), 1);
        }

        /* 非特殊会员组检查积分的上下限是否合理 */
        if ($_POST['min_points'] >= $_POST['max_points'] && $special_rank == 0)
        {
            sys_msg($this->_LANG['js_languages']['integral_max_small'], 1);
        }

        /* 特殊等级会员组不判断积分限制 */
        if ($special_rank == 0)
        {
            /* 检查下限制有无重复 */
            if (!$this->exc->is_only('min_points', intval($_POST['min_points'])))
            {
                sys_msg(sprintf($this->_LANG['integral_min_exists'], intval($_POST['min_points'])));
            }
        }

        /* 特殊等级会员组不判断积分限制 */
        if ($special_rank == 0)
        {
            /* 检查上限有无重复 */
            if (!$this->exc->is_only('max_points', intval($_POST['max_points'])))
            {
                sys_msg(sprintf($this->_LANG['integral_max_exists'], intval($_POST['max_points'])));
            }
        }

        $sql = "INSERT INTO " .$this->ecs->table('user_rank') ."( ".
            "rank_name, min_points, max_points, discount, special_rank, show_price".
            ") VALUES (".
            "'$_POST[rank_name]', '" .intval($_POST['min_points']). "', '" .intval($_POST['max_points']). "', ".
            "'$_POST[discount]', '$special_rank', '" .intval($_POST['show_price']). "')";
        $this->db ->query($sql);

        /* 管理员日志 */
        admin_log(trim($_POST['rank_name']), 'add', 'user_rank');
        clear_cache_files();

        $lnk[] = array('text' => $this->_LANG['back_list'],    'href'=>'user_rank.php?act=list');
        $lnk[] = array('text' => $this->_LANG['add_continue'], 'href'=>'user_rank.php?act=add');
        sys_msg($this->_LANG['add_rank_success'], 0, $lnk);
    }

    /*------------------------------------------------------ */
//-- 删除会员等级
    /*------------------------------------------------------ */
    public function remove ()
    {
        check_authz_json('user_rank');

        $rank_id = intval($_GET['id']);

        if ($this->exc->drop($rank_id))
        {
            /* 更新会员表的等级字段 */
            $this->exc_user->edit("user_rank = 0", $rank_id);

            $rank_name = $this->exc->get_name($rank_id);
            admin_log(addslashes($rank_name), 'remove', 'user_rank');
            clear_cache_files();
        }

        $url = 'user_rank.php?act=query&' . str_replace('act=remove', '', $_SERVER['QUERY_STRING']);

        ecs_header("Location: $url\n");
        exit;

    }
    /*
     *  编辑会员等级名称
     */
    public function edit_name ()
    {
        $id = intval($_REQUEST['id']);
        $val = empty($_REQUEST['val']) ? '' : json_str_iconv(trim($_REQUEST['val']));
        check_authz_json('user_rank');
        if ($this->exc->is_only('rank_name', $val, $id))
        {
            if ($this->exc->edit("rank_name = '$val'", $id))
            {
                /* 管理员日志 */
                admin_log($val, 'edit', 'user_rank');
                clear_cache_files();
                make_json_result(stripcslashes($val));
            }
            else
            {
                make_json_error($this->db ->error());
            }
        }
        else
        {
            make_json_error(sprintf($this->_LANG['rank_name_exists'], htmlspecialchars($val)));
        }
    }

    /*
     *  ajax编辑积分下限
     */
    public function edit_min_points ()
    {
        check_authz_json('user_rank');

        $rank_id = empty($_REQUEST['id']) ? 0 : intval($_REQUEST['id']);
        $val = empty($_REQUEST['val']) ? 0 : intval($_REQUEST['val']);

        $rank = $this->db ->getRow("SELECT max_points, special_rank FROM " . $this->ecs->table('user_rank') . " WHERE rank_id = '$rank_id'");
        if ($val >= $rank['max_points'] && $rank['special_rank'] == 0)
        {
            make_json_error($this->_LANG['js_languages']['integral_max_small']);
        }

        if ($rank['special_rank'] ==0 && !$this->exc->is_only('min_points', $val, $rank_id))
        {
            make_json_error(sprintf($this->_LANG['integral_min_exists'], $val));
        }

        if ($this->exc->edit("min_points = '$val'", $rank_id))
        {
            $rank_name = $this->exc->get_name($rank_id);
            admin_log(addslashes($rank_name), 'edit', 'user_rank');
            make_json_result($val);
        }
        else
        {
            make_json_error($this->db ->error());
        }
    }

    /*
     *  ajax修改积分上限
     */
    public function edit_max_points ()
    {
        check_authz_json('user_rank');

        $rank_id = empty($_REQUEST['id']) ? 0 : intval($_REQUEST['id']);
        $val = empty($_REQUEST['val']) ? 0 : intval($_REQUEST['val']);

        $rank = $this->db ->getRow("SELECT min_points, special_rank FROM " . $this->ecs->table('user_rank') . " WHERE rank_id = '$rank_id'");

        if ($val <= $rank['min_points'] && $rank['special_rank'] == 0)
        {
            make_json_error($this->_LANG['js_languages']['integral_max_small']);
        }

        if ($rank['special_rank'] ==0 && !$this->exc->is_only('max_points', $val, $rank_id))
        {
            make_json_error(sprintf($this->_LANG['integral_max_exists'], $val));
        }
        if ($this->exc->edit("max_points = '$val'", $rank_id))
        {
            $rank_name = $this->exc->get_name($rank_id);
            admin_log(addslashes($rank_name), 'edit', 'user_rank');
            make_json_result($val);
        }
        else
        {
            make_json_error($this->db ->error());
        }
    }

    /*
     *  修改折扣率
     */
    public function edit_discount ()
    {
        check_authz_json('user_rank');

        $rank_id = empty($_REQUEST['id']) ? 0 : intval($_REQUEST['id']);
        $val = empty($_REQUEST['val']) ? 0 : intval($_REQUEST['val']);

        if ($val < 1 || $val > 100)
        {
            make_json_error($this->_LANG['js_languages']['discount_invalid']);
        }

        if ($this->exc->edit("discount = '$val'", $rank_id))
        {
            $rank_name = $this->exc->get_name($rank_id);
            admin_log(addslashes($rank_name), 'edit', 'user_rank');
            clear_cache_files();
            make_json_result($val);
        }
        else
        {
            make_json_error($val);
        }
    }

    /*------------------------------------------------------ */
//-- 切换是否是特殊会员组
    /*------------------------------------------------------ */
    public function toggle_special ()
    {
        check_authz_json('user_rank');

        $rank_id       = intval($_POST['id']);
        $is_special    = intval($_POST['val']);

        if ($this->exc->edit("special_rank = '$is_special'", $rank_id))
        {
            $rank_name = $this->exc->get_name($rank_id);
            admin_log(addslashes($rank_name), 'edit', 'user_rank');
            make_json_result($is_special);
        }
        else
        {
            make_json_error($this->db ->error());
        }
    }
    /*------------------------------------------------------ */
//-- 切换是否显示价格
    /*------------------------------------------------------ */
    public function toggle_showprice ()
    {
        check_authz_json('user_rank');

        $rank_id       = intval($_POST['id']);
        $is_show    = intval($_POST['val']);

        if ($this->exc->edit("show_price = '$is_show'", $rank_id))
        {
            $rank_name = $this->exc->get_name($rank_id);
            admin_log(addslashes($rank_name), 'edit', 'user_rank');
            clear_cache_files();
            make_json_result($is_show);
        }
        else
        {
            make_json_error($this->db ->error());
        }
    }

}

global $ecs,$db;
$exc = new exchange($ecs->table("user_rank"), $db, 'rank_id', 'rank_name');
$exc_user = new exchange($ecs->table("users"), $db, 'user_rank', 'user_rank');
