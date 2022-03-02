# Ubidots 脚本

## Curl
Curl 是一个开源命令行工具。例如， HTML 消息可以发送到 Ubidots.- https://curl.haxx.se/

## Ubidots API
许多 Ubidots 的功能可以通过 HTML-messages 进行脚本控制。
例如，当一个变量不再需要时，可以完全删除它的数据。
- https://ubidots.com/docs/api/#rest-api-reference

## Curl 脚本 用于数据擦除
- 下载 [压缩包](https://github.com/universam1/iSpindel/raw/master/tools/delete_all_values_ispindel.zip) 
- 随即解压文件
- 使用文本编辑器编辑里面的 "delete_all_values_ispindel.bat" ，然后替换成

  `<ID of your variable for Ubidots>`
  
  清空 ID of the variable, 使用你 Ubidots 上的进行代替
  
  `<Token of your account at Ubidots>`
  
  你的密钥。
- 当然你可以随意多次复制该行，如果你想同时清空几个变量的话。
- 启动 "delete_all_values_ispindel.bat" 然后 variable (s) 应该会被清空

## Curl 脚本显示最后一次测量值
- 下载 [压缩包](https://github.com/universam1/iSpindel/raw/master/tools/get_five_values_ispindel.zip) 
- 随即解压文件
- 使用你文本编辑器编辑里面的 "get_five_values_ispindel.bat" ，然后替换成

  `<ID of your variable for Ubidots>`
  
  这个 ID of the variable，你能在 Ubidots 上找到并代替
  
  `<Token of your account at Ubidots>`
  
  你的密钥。
- 当然你可以随意多次复制该行，如果你想同时清空几个变量的话。
- 启动 "get_five_values_ispindel.bat" 读取最后五个 variable (s) 数值。