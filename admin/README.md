面向过程到面向对象:
通过IDE的正则表达式替换：

    elseif\s+\(\$_REQUEST.*?\s==\s[\'\"]([\w\d_]+)[\'\"]\)
    public function $1()
    
    (else)?if\s*\(\$_REQUEST\[.*==\s*[\'\"]([\w\d_]+)[\'\"]\)
    public function $2 ()