# Задание 2
1. Выданный [файлик](https://github.com/n-popov/nspde_hws/blob/main/ex1.cpp) модифицирован так, чтобы можно было удобно измерить время работы программы.

Для измерения времени работы реализован класс RAII-таймера.

```
template<typename precision>
class Timer {
public:
	Timer(): begin(std::chrono::steady_clock::now()) {}
  
	~Timer() {
		std::cout << std::chrono::duration_cast<precision>(std::chrono::steady_clock::now() - begin).count() << std::endl;
  }
  
	std::chrono::steady_clock::time_point begin;
};

```

За увеличение мелкости сетки отвечала родная функция UniformRefinement:

```
for (int l = 0; l < refs; l++) {
            mesh->UniformRefinement();
        }        
```

2. Измерено время работы в мкс, при этом затраты на Refinement не учитываются:

| N | time, mus    |
|---|-------------|
| 20      | 735      |
| 80      | 1732     |
| 320     | 1386     |
| 1280    | 6443     |
| 5120    | 31557    |
| 20480   | 149426   |
| 81920   | 755328   |
| 327680  | 4373082  |
| 1310720 | 37018850 |

Получилась зависимость, похожая скорее на квадратичную, чем на линейную, что, наверное, должно быть очевидно, так как сетка двумерная.
Приведём графики в [линейном](https://github.com/n-popov/nspde_hws/blob/main/result.png) и [логарифмическом по размеру сетки](https://github.com/n-popov/nspde_hws/blob/main/result_logarithmic.png) масштабе.

![Линейный](https://github.com/n-popov/nspde_hws/blob/main/result.png)
![Логарифмический](https://github.com/n-popov/nspde_hws/blob/main/result_logarithmic.png)

3. Также в файлике реализована функция f_right и добавлена в расчёт:
```
const auto f_right = [](const mfem::Vector &v) {
	return v(0) * v(1);
};
```

```
FunctionCoefficient b_function(f_right);
    		b.AddDomainIntegrator(new DomainLFIntegrator(b_function));
```
