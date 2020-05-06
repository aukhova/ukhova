1) С сайта https://opencv.org/releases/ копируем исходники opencv-3.4.9.zip 
2) С сайта https://github.com/opencv/opencv_contrib/releases копируем исходники версии 3.4.9 -- opencv_contrib-3.4.9.zip 
3) На диске С создаем директорию opencv, в ней созданем директорию build
4) В директорию opencv разворичиваем архивы, получаем слудеющую структуру директорий:
c:
	build 
	opencv-3.4.9 
	opencv_contrib-3.4.9
5) Для visual studio 2017 запускаем MSBuild x64 Command Prompt. Это сборка проектов из командной строки.
6) Из Командной строки MSBuild запускаем cmake-gui
7) В cmake-gui задаем
	директория сборки c:\build 
	исходные файлы opencv c:\opencv-3.4.9 
	в параметрах ищем OPENCV_EXTRA_MODULES_PATH и в значении параметра задаем c:\opencv_contrib-3.4.9
	для параметре OPEN_CV_NON_FREE ставим галочку
8) Далее жмем кнопку configure , затем generate и после open project. В качестве среды сборки 
выбираем visual studio 2017
9) В настройках меню Build->Configure Manager проставляем галочку install.
9) Пересобирем проект.
10) В результате пересборки в директории
C:\opencv\build\install\ 
будет находится полная сборка opencv + openc_contrib, ее мы и будем использовать для компиляции проекта.

11) Исходные файлы проекта взыты с сайта:
http://itnotesblog.ru/note.php?id=271




		