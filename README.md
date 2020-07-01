# 第二周大作业

## 介绍

我们本次作业的目的是用 C语言 完成一个包含图分析与算法的代码库，包括
- 图元素的基础分析
- 顶点与边基础统计
- 图度量
- 边度量等
- 最短路径蒜法等功能。

再将此代码库制作成一个 CLI 工具，可以直接在 Linux 的 Terminal 里面运行。


## 具体要求
- C 语言实现，只允许调用 `stdlib.h` 和 `stdio.h`（**0.5%**）
  - 其余函数如有需要，请自行实现，不做限制
  - 可使用网上的资源，但请在文件注释中注明来源

- 编辑 `stats.h` 和 `stats.c` 文件，完成如下函数（**2%**）：
  - `int numberOfEdges(char name[])` 接受以文件名为图标识符的 `char` 数组，返回图中边的数量
  - `int numberOfVertices(char name[])` 接受以文件名为图标识符的 `char` 数组，返回图中顶点的数量
  - `int freemanNetworkCentrality(char name[])` 接受以文件名为图标识符的 `char` 数组，返回图中 Freeman's Network Centrality 值
  - `int closenessCentrality(char name[])` 接受以文件名为图标识符的 `char` 数组，返回图中 Closeness Centrality 值
  - 参考 https://www.cl.cam.ac.uk/teaching/1213/L109/stna-lecture3.pdf
  - 评分标准为
	- 前**两个**函数 + 后面两个函数中选**一个**
	- **3/3 = 100% \* 2% = 2%，4/3 = 125% \* 2% = 2.5%**


- 将 `DFS、BFS、Dijkstra` 三个算法封装在 `search.h` 和 `search.c` 文件中（**3%，0.75% + 0.75% + 1.5%**）
  - 每个函数封装结构没有要求，输入输出也没有要求
  - 需要额外加入一个函数 `int* shortestPath(int u, int v, char algorithm[])`用来评分
	- 输入两个 `int` 值，`u` 和 `v`，分别为起点与终点的顶点编号
	- 输入一个 `char` 数组，装有所选算法的名字（分别为：`{DFS, BFS, Dijkstra}`， 请严格遵守）
	- 输出一个 `int` 数组指针，装有从 `u` 到 `v` 的最短路径
  - 我们将对这三个算法进行检查评分，并提供简单的数据及其 autograder 让你们本地自检


- 编辑 `main.c` 文件，实现以下功能(**1%**)：
  - 输出二进制文件名为 `search-cli`
  - 利用 main 函数中的 argv 和 argc 参数接受命令行中的参数，完成以下命令行功能：
	- `./search-cli -h/--help`(-h或--help，以下同) 显示帮助菜单，格式没有要求，不作为评分要求，只求自己能够看懂
	- `./search-cli -g/--graph FILE_PATH -s/--stats STATS_PARAMS` 显示以 `FILE_PATH` 为输入文件的图的统计信息，输出没有格式要求，具体支持的`STATS_PARAMS`参数如下：
	  - `edges`
	  - `vertices`
	  - `freeman`
	  - `betweenness`
	  - `closeness`
	  - `eigenvector`
	- `./search-cli -g/--graph FILE_PATH -sp/--shortestpath SEARCH_PARAMS -u STARTING_POINT -v TARGET_POINT` 显示以 `FILE_PATH` 为输入文件的图中 从开始点 u 到 终点 v 的用 `SEARCH_PARAMS`蒜出来的最短路径
	  - 样例输入： `./search-cli -g ./sx-stackoverflow.txt -sp Dijkstra -u 1 -v 5`
	  - 样例输出： `1 -> 2 -> 3 -> 4 ->5`
- 自测文件为 `test.txt`，检测所写算法能否在小的数据集上运行正确，具体文件格式如下：
  - 第一行：`n m` 代表n个顶点，m条边
  - 接下来每行： `u v w` 代表从`u`到`v`的权重为`w`的边
  - 正确答案保存在`test_key.txt` 注：只有唯一最优解


- `Makefile`
  - 目标二进制文件为： `search-cli`
  - 不规定具体编译路径
  - 样例`Makefile`只是一个骨架，不能直接运行不能直接运行不能直接运行！！！


- 真正的测试集来源于斯坦福开放的图数据集(**1%**)（https://snap.stanford.edu/data/amazon0601.txt.gz）
  - 具体信息参见 https://snap.stanford.edu/data/amazon0601.html
  - 将会提供修改版数据集（简化），测试也将在修改版上进行


- 加分项
  - **0.1%** `./search-cli -j` 可以在 Terminal 中画出一副你想象中蒜头君的图（如图所示）
![](https://tva1.sinaimg.cn/large/007S8ZIlgy1ggb7i8vm01j30l30drwg3.jpg)


已完成：
```
 ______________________________________
< 想不到吧，蒜头君是一头牛 >
 --------------------------------------
        \   ^__^
         \  (oo)\_______
            (__)\       )\/\
                ||----w |
                ||     ||
```
  - **2%** 更大的数据集，来自于 https://snap.stanford.edu/data/sx-stackoverflow.html
	- \~260万个顶点 \~6400万条边
	- 需要更好的优化（内存，时间复杂度）
  - **0.1%** `Makefile` 中加入 `make clean`
  - 待更新


- 目录结构 - 可增加新文件，此目录下所有文件是批改所需，请不要改动文件名
	  GraphProject/main.c
		           search.c
		           search.h
		           stats.c
		           stats.h
		           dummy_test.txt
		           dummy_test_key.txt
		           Makefile
		           submission.url


- 交作业方式
	  - 与 Debug 作业相同
	  - 原则上不更换小组
	  - 每小组开一个新的 `repo`，名字叫做 `GraphProject`
	  - 将小组的 `repo url` 填入助教提供的表格中，并将助教的`GitHub account` 添加为 `collaborator`

