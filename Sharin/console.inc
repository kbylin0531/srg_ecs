<?php
namespace {

    use Sharin\ClassLoader;

    if(PHP_SAPI !== 'cli') die('Please run as client mode!');

    require __DIR__.'/Common/constant.inc';
    require __DIR__.'/Common/debug_suit.inc';
    require __DIR__.'/Core/bundle.inc';

    ClassLoader::register();

}