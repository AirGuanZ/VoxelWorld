# VoxelWorld

---

基于DirectX 11实现。使用DirectXTK加载纹理；使用AssImp加载骨骼动画；使用ImGui进行GUI显示和交互。

---

以minecraft世界为原型，未来将添加更多内容。

- [x] 太阳光和彩色光源
- [x] 根据摄像机的位置动态加/卸载区块
- [x] 环境光遮蔽
- [x] 日夜变换
- [x] 碰撞检测
- [x] 雾效果
- [x] 基于维诺图 + 柏林噪声的世界生成
- [x] 方块破坏与放置
- [x] 第三人称角色的移动和跳跃
- [ ] 自定义角色形象（施工中）
- [ ] 洞穴等地形结构

以鼠标移动来转动镜头，`WASD`移动，`Space`跳跃。鼠标左键破坏方块，右键放置萤石。日夜变换周期较长，按住`T`可以加速这一过程。

修改项目目录下的`config.txt`可以调整运行时参数，包括窗口分辨率、可视距离、雾距离等。

---

截图：

![Screenshot](Screenshots/0.png)

![Screenshot](Screenshots/1.png)

![Screenshot](Screenshots/2.png)
