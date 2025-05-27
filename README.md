# Reloj Despertador Digital – Electrónica Digital III (2025-1)

Este proyecto implementa un reloj despertador digital diseñado como ejemplo para el curso de Electrónica Digital III. El sistema permite mostrar la hora, configurar una alarma con múltiples modos de repetición y visualizar la fecha y el día de la semana. Toda la interacción se realiza mediante botones físicos y se presenta en una interfaz de 7 segmentos. El sistema está basado en una Raspberry Pi Pico (RP2040) y utiliza su RTC interno para mantener el tiempo incluso ante interrupciones breves de energía.

## Descripción Funcional

El reloj ofrece las siguientes funcionalidades:
- Visualización continua de la hora en formato 24 horas.
- Ajuste manual de hora y fecha mediante botones.
- Configuración de una alarma con repetición diaria, semanal o en una fecha específica.
- Activación de un buzzer al alcanzar la hora de la alarma.
- Posibilidad de posponer la alarma (función snooze) por un intervalo configurable.
- Visualización temporal de la fecha y día de la semana mediante botón dedicado.
- Fuente de respaldo que mantiene la hora y configuración en caso de apagones breves.
- Lógica de pulsación corta y larga para ingresar a los modos de configuración.

---

## Requisitos Funcionales

- **RF01** – Mostrar hora en formato 24h con displays de 7 segmentos.
- **RF02** – Ajuste manual de hora y fecha mediante botones físicos.
- **RF03** – Configuración de una única alarma con repetición diaria, semanal o fecha específica.
- **RF04** – LED indicador del estado de la alarma.
- **RF05** – Activación de buzzer al cumplirse la hora de la alarma.
- **RF06** – Apagar la alarma mediante una pulsación corta.
- **RF07** – Posponer la alarma mediante un botón dedicado; el intervalo es configurable.
- **RF08** – Mantener hora y fecha en cortes breves de energía con RTC interno.
- **RF09** – Visualización temporal de fecha y día de la semana mediante botón.
- **RF10** – Acceso a modos de configuración mediante pulsaciones cortas/largas.

---

## Requisitos No Funcionales

- **RNF01** – Precisión de ±1 minuto por semana.
- **RNF02** – Latencia de respuesta a pulsaciones menor a 1 segundo.
- **RNF03** – Interfaz intuitiva y coherente sin necesidad de instrucciones externas.
- **RNF04** – Consumo eléctrico < 1 W en operación normal (excluyendo buzzer).
- **RNF06** – Recuperación automática tras corte de energía si RTC no perdió alimentación.
- **RNF07** – Código estructurado en capas funcionales (drivers, abstracción, lógica de aplicación).

---

## Especificaciones Técnicas

### Hardware

- **Plataforma base**: Raspberry Pi Pico (RP2040)
- **Alimentación principal**: 5 V regulados.
- **Fuente de respaldo**: Batería recargable LiPo (18650) conectada a un circuito de detección para preservar RTC durante apagones. El GPIO `VMAIN_OK` detecta la caída de tensión.
- **RTC**: Módulo interno del RP2040, alimentado continuamente por la fuente de respaldo.
- **Display**:
  - 4 displays de 7 segmentos (configuración multiplexada).
  - Control de segmentos: 1 slice PWM como base de tiempo a 250 Hz.
  - Indicador decimal opcional conectado a GPIO adicional.
- **LED indicador**:
  - LED conectado a GPIO (p. ej. GP15), configuración sourcing con resistencia limitadora (~330 Ω).
  - Indica si hay una alarma activa.
- **Buzzer**:
  - Piezoeléctrico activo conectado a GPIO (p. ej. GP14).
  - Activación por GPIO simple o PWM si se desea tono definido.
  - Duración máxima de sonido: 60 segundos.
- **Botones físicos** (todos con detección por flanco descendente y pull-ups internos):
  - `BTN_SET_TIME`: Configurar hora y fecha.
  - `BTN_SET_ALARM`: Configurar alarma / apagar alarma.
  - `BTN_PLUS`: Incrementar valores.
  - `BTN_MINUS`: Decrementar valores.
  - `BTN_SNOOZE`: Posponer alarma / configurar intervalo.
  - `BTN_SHOW_DATE`: Mostrar fecha actual y día de la semana.

### Software / Firmware

- **Manejo del tiempo**:
  - El RTC se inicia por defecto con hora 12:00 del 15/06/2025 si no hay configuración previa.
  - Lectura y actualización del RTC cada segundo.
- **Visualización**:
  - Refresco multiplexado del display a ≥ 240 Hz.
  - Parpadeo de campo activo durante configuraciones (controlado por PWM).
- **Configuraciones**:
  - Entrada a modos mediante secuencias de pulsaciones (corta / larga).
  - Máquina de estados para configurar hora, fecha y alarma.
  - Validación de fechas inválidas (e.g., 31/02).
  - Inactividad >30 s en modo configuración cancela los cambios.
- **Modo DORMANT**:
  - Activado ante pérdida de `VMAIN_OK`.
  - Apaga periféricos, LEDs y PWM; solo RTC permanece activo.
  - El sistema reanuda automáticamente y evalúa alarmas pendientes si la restauración ocurre en menos de 30 minutos.
- **Snooze**:
  - Posposición configurable entre 1 y 30 minutos.
  - Valor por defecto: 5 minutos.
  - No aplica a alarmas de tipo único ya ejecutadas.
- **Manejo de alarmas**:
  - Se utiliza la alarma del RTC interno para evitar comparaciones por software.
  - Tipos de repetición soportados: diaria, semanal, fecha única.
  - Una única alarma activa al mismo tiempo.
- **Interfaz de usuario**:
  - Antirrebote por software (≥20 ms).
  - Lógica contextual según estado del sistema (normal, configuración, alarma sonando).
  - Interrupciones prioritarias para estados SONANDO.

---

## Licencia

Este proyecto se proporciona con fines educativos. Puedes modificar y adaptar el código conforme a los objetivos del curso o tus necesidades de aprendizaje.
