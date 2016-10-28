#!/usr/bin/env php
<?php
namespace {
    use Sharin\Core\Storage;
    use Sharin\Exception;
    use Sharin\Library\Helper\StringHelper;
    require __DIR__.'/console.inc';

    //预定义
    $sample_tail = '.sample';
    $template_dir = __DIR__.'/Install/';
    $target_dir = realpath(SR_PATH_BASE);
    //设置访问访问
    Storage::setConvention('WRITABLE_SCOPE',SR_PATH_BASE);
    is_dir($target_dir) or Storage::mkdir($target_dir);

    $counter = [
        'fi'     => 0,
        'fj'     => 0,
        'di'     => 0,
        'dj'     => 0,
    ];

    //读取并遍历源目录
    $files = Storage::readdir($template_dir,true);
    foreach ($files as $name => $file){
        if(is_file($file)){
            $content = Storage::read($file);
            //只有带上尾巴才能被拷贝
            if(StringHelper::endWith($name,$sample_tail)){//判断是否带有尾巴
                $name = strstr($name,$sample_tail,true);//删除尾巴
                $newfile = $target_dir.$name;
                if(!is_file($newfile)){
                    if(!Storage::write($target_dir.$name,$content)) {
                        throw new Exception("Write file '$newfile' failed");
                    }
                    $counter['fi'] ++;
                }else{
                    //文件已经存在则略过
                    $counter['fj'] ++;
                }
            }
        }elseif(is_dir($file)){
            $newfile = $target_dir.$name;
            if(!is_dir($newfile)){
                //文件夹可能重复，文件不会
                if(!Storage::mkdir($newfile)){
                    throw new Exception("Make Folder '$newfile' failed");
                }
                $counter['di'] ++;
            }else{
                //目录已经存在则跳过
                $counter['dj'] ++;
            }
        }else{
            throw new Exception("Unknown file '{$name}' !");
        }
    }

    echo "--------------------Done!-----------------------
    File Success: {$counter['fi']}   Failed: {$counter['fj']}
    Folder Success: {$counter['di']}   Failed: {$counter['dj']}
    Failed may due to exist of that file!\n------------------------------------------------\n";

}