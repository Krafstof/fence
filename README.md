Билд собирался под Desktop Qt 6.9.3 MinGW
В качестве оптимизации использовался буфер для уже сгенерированного изображения, что позволило ускорить выполнение на высоких параметрах автовыполнения
Пример до оптимизации (флаг useOptimized = false):
```
AUTO RATE: 25.9578 updates/sec
AUTO RATE: 46.594 updates/sec
AUTO RATE: 56.2577 updates/sec
AUTO RATE: 44.08 updates/sec
AUTO RATE: 32.4183 updates/sec
```
Пример после оптимизации (флаг useOptimized = true):
```
AUTO RATE: 18.383 updates/sec
AUTO RATE: 34.2466 updates/sec
AUTO RATE: 48.0769 updates/sec
AUTO RATE: 60.2319 updates/sec
AUTO RATE: 70.9925 updates/sec
AUTO RATE: 80.5542 updates/sec
AUTO RATE: 89.0631 updates/sec
AUTO RATE: 96.14 updates/sec
AUTO RATE: 102.008 updates/sec
AUTO RATE: 107.793 updates/sec
AUTO RATE: 112.788 updates/sec
AUTO RATE: 116.527 updates/sec
AUTO RATE: 119.168 updates/sec
```
Скорость не деградирует в процессе выполнения
Тесты лежат в отдельном проекте tests
