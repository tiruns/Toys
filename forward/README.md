# forward

forward的工作方式类似于快捷方式，每一个forward程序会调用另一个特定的程序并传递所有它接收到的参数

可以使用forward将多个需要添加到PATH环境变量的程序集中到一处，从而降低冲突发生的概率，也避免了反复修改环境变量

使用方法：
```
python build_forward.py [.exe绝对路径]
```