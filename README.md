# Proyecto JIC: Sistema de Tutoría Inteligente Adaptativa (STI)

Este proyecto desarrolla un sistema de tutoría inteligente diseñado para personas con aprendizaje lento, enfocado en la alfabetización básica y matemáticas elementales. El sistema utiliza modelos probabilísticos para adaptar la dificultad de forma invisible y silenciosa al ritmo del usuario.

## Arquitectura y Modularidad

El sistema se basa en una modularidad total, donde cada capa es reemplazable e independiente.

   -  UI (Presentación): Desarrollada en Python + Tkinter. Maneja la visualización de ejercicios y captura de respuestas sin contener lógica de negocio.
   -  Orquestador: Coordina la comunicación entre capas, procesa eventos de la UI, actualiza el modelo BKT y gestiona la persistencia.
   -  Motor de Contenido: Gestiona la carga de ejercicios desde archivos JSON y los filtra por habilidad y dificultad.
   -  Modelo Adaptativo (BKT): Implementa el motor de inferencia probabilística. Se contempla el uso de C++20 si se requiere optimización de rendimiento crítica.
   -  Persistencia (SQLite): Encargada de la lectura y escritura del estado del usuario y logs de respuesta.

🛠️ Estándares de Desarrollo
1. Nomenclatura y Estilo

    Variables y Funciones (Python): Se debe utilizar snake_case (ej. update_mastery_probability).
   
    Clases: Se debe utilizar PascalCase (ej. BKTModel).

    Constantes: Se definen en UPPER_CASE (ej. MAX_SRS_PERCENTAGE).

    Tipado: En Python es obligatorio el uso de Type Hinting para asegurar la claridad técnica.

3. Documentación y Comentarios

    Comentarios de "Por qué": No documentar qué hace el código (el código debe ser auto-explicativo), sino la razón técnica o matemática del diseño.

    Fundamento Matemático: Cualquier modificación en el motor adaptativo debe referenciar el parámetro BKT afectado (P(L0​), P(T), P(G), P(S), P(F)) .

    Ecuaciones: Utilizar LaTeX para documentar las actualizaciones de probabilidad en el código.
    P(L∣correcto)=P(L)⋅(1−P(S))+(1−P(L))⋅P(G)P(L)⋅(1−P(S))​

4. Reglas de Refactorización

Para mantener el mantra de optimización y seguridad, toda refactorización debe seguir este protocolo:
- No refactorizar sin justificación: Se debe comparar la versión anterior con la nueva.
- Enlistar mejoras: Detallar si el cambio es sintáctico, lógico o performático.
- Explicación técnica: Explicar por qué los nuevos conceptos son superiores al diseño actual.

## Lógica Adaptativa

El sistema implementa un modelo Bayesian Knowledge Tracing (BKT) Extendido con cinco parámetros, diseñado para operar eficazmente desde el primer uso.

    Zone Blending: Selecciona la mezcla de ejercicios (bajo, medio, alto) según la probabilidad de dominio P(L).

    SRS (Spaced Repetition): Gestiona la revisión de habilidades previas con frecuencia decreciente, limitando la revisión al 30% de la sesión.

    Penalización por Tiempo: La combinación de respuesta lenta e incorrecta activa una reducción del parámetro P(G) para reflejar un fallo en el procesamiento activo.

## Roadmap de Implementación

    Fase 1 (Datos): Definición del skill_graph.json y banco de ejercicios.

    Fase 2 (Persistencia): Implementación de CRUD en SQLite.

    Fase 3 (BKT): Lógica de 5 parámetros y tests de convergencia.

    Fase 4 (Motores): Implementación de Zone Blender y SRS.

    Fase 5 (Integración): Orquestador y flujo completo sin interfaz.

    Fase 6 (UI): Desarrollo de la interfaz gráfica y sistema de feedback.
