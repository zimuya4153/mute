# mute - 简单的禁言插件

## 命令
> [] 为可选   <> 为必选
- `/mute player <玩家> [禁言时间]` - 禁言玩家
- `/mute all [禁言时间]` - 全体禁言
> 禁言时间如果不填，则为解除禁言(填负则永久)   

> 禁言时间后面可以跟`s(秒)`,`m(分)`,`h(时)`,`d(天)`   
> 如`/mute all 10d`,这是全体禁言10天

## 命令使用实例
- `/mute all` - 解除全体禁言
- `/mute all -1` - 全体永久禁言
- `/mute all 1m` - 全体禁言1分钟
- `/mute player zimuya4153` - 解除玩家zimuya4153的禁言
- `/mute player zimuya4153 0` - 解除玩家zimuya4153的禁言
- `/mute player zimuya4153 1d` - 禁言玩家zimuya4153一天

## 配置文件
```jsonc
{
    "version": 1, // 配置文件版本(勿动)
    "all": 0, // 全体禁言解除的时间戳(建议使用命令控制)
    "playerData": {}, // 玩家禁言数据 键名为玩家uuid 值为解禁时间戳建议使用命令控制)
    "disabledCmd": [ // 禁言时禁用的命令
        "me",
        "msg",
        "tell",
        "w"
    ],
    "disabled": { // 禁言时禁止的操作
        "book": true, // 编辑书与笔
        "sign": true // 修改告示牌内容
    },
    "command": { // 命令相关(不懂勿动)
        "command": "mute", // 主命令
        "alias": "", // 命令别名
        "permLevel": "GameDirectors" // 命令权限
    }
}
```