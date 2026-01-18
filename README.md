# CG 大作业

路径追踪大作业，直接在 VS2022 中编译即可

RayTrace 为我实现的路径追踪

光子数量不能在运行界面调整，需要在 `components/my_ray_tracing/include/photonmapping/photonmapping.hpp` 中手动调整。

查找光子的半径以及其他参数可以在材料属性中修改。

默认使用 Phong 模型进行渲染，可以用属性 ShaderType 进行修改，0123分别对应 Lambertian, Phong, Dielectric, PBR 四种模型。

测试用的模型在 resources 文件夹中

