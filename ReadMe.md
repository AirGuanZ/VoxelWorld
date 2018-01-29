# Voxel World

---

基于DirectX 11实现，使用DirectXTK来加载纹理。

---

以minecraft世界为原型，未来将添加更多内容。

- [x] 半透明方块渲染
- [x] 简单的世界生成，包括山丘、水域和花草树木
- [x] 光照计算，包括太阳光和彩色光源
- [x] 根据摄像机的位置动态加/卸载区块
- [x] 环境光遮蔽
- [x] 日夜变换
- [x] 碰撞检测
- [x] 雾效果
- [ ] 物品栏

以鼠标移动来转动镜头，`WASD`移动，`Space`上升，`LeftShift`下降。鼠标左键破坏方块，右键放置红色萤石。日夜变换周期较长，按住`T`可以加速这一过程。

修改项目目录下的`config.txt`可以调整运行时参数，包括窗口分辨率，多重采样，可视距离，雾距离等。

---

截图：

![Screenshot](Screenshots/0.png)

![Screenshot](Screenshots/1.png)

![Screenshot](Screenshots/2.png)
