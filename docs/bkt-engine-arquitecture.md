### Separacion ```struct SkillSate``` y ```class BKTEngine```:
- En C++ moderno, un struct con miembros publicos sirve como aggregate o value object. Facilitara la serializacion directa a SQLite/JSON, el paso por referencia sin overhead de getters/setters y la clonacion de pruebas deterministas.
- El BKTEngine encapsulara las logicas de calculo, permitira inyeccion de configuracion futura para poder expandir en funcionamientos la base del modelo y evita un estado mutable compartido.

### Tipos de datos y la precision:
- Para las variables de los pesos del modelo [ P(l) | P(G) | P(T) | ... ] se usa double para evitar los drift numericos que ocasionaria el uso de float.
- Para los contadores usamos ```uint_32```  para tener un rango suficiente (4.29×10⁹), semántica sin signo, alineación natural en caché. Evita desbordamientos en sesiones prolongadas o bots de estrés.
- la ```skill_id``` en formato string sera clave de partición para ```PersistenceLayer``` y aislamiento por habilidad. Compatible con ```sqlite3_bind_text``` y ```nlohmann::json```.

### Gestion del tiempo:
- Usamos ```std::chrono::system_clock``` en variables de tiempo que deben ser persistentes y comparables entre reinicios del sistema. ```system_clock``` mapea a tiempo UNIX, ideal para guardar en SQLite y calcular horas de inactividad.
- Usamos ```std::chrono::steady_clock``` en variables de metricas de tiempo usado solo para decaimiento intra-sesión. ```steady_clock``` es monótono: no se ve afectado por cambios de zona horaria, NTP o hibernación del SO. 

### Constantes de configuracion:
- ```constexpr``` a nivel del namespace, permite que los miembros del namespace puedan acceder a los valores constantes declarados sin necesidad de crear instancias, centralizarlos permitira modificar los parametros sin necesidad de tocar la logica del motor.
- ```std::chrono::hour``` tipado fuerte,evita errores a la hora de hacer operaciones comparativas con ```duration_cast```
- ``[[nodiscard]]`` en ``setters`` obliga a usarlos al llamar al valor, si son ignorados en el proceso se pierde gran parte de la funcionalidad y logica del modelo.
- ``const`` en parametros de entrada, evita mutaciones o copias accidentales y habilita optimizaciones de aliasing.

### Validacion y seguridad:
- ``std::clamp`` Implementa explícitamente el 0.01 <= techo <= 0.98  y pisos de seguridad. Se invoca post-update para no acoplar validación a mutación incidental.
- Inicializacion en ```{0.0}/{}``` permite un estado predecible para cold-start, deserialización desde SQLite y construcción por defecto en tests unitarios.
