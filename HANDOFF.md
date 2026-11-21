# HANDOFF - Chui's Dreamcast Projects (archivo)

Estado a 2026-11-21. Para quien retome el trabajo (persona o IA).

## Qué es
Copia de preservación de la antigua web de emuladores de Dreamcast de Chui. Una carpeta por proyecto con su HTML, su carpeta `archivos/` (recursos de la web y paquetes descargables) y, desde el 2026-11-21, su carpeta `src/` con el código fuente descomprimido.

## Hecho
1. HTML apuntan a `./archivos/` (antes a `./Chui's DC projects - X_files/`).
2. Enlaces internos a páginas y descargas apuntan a ficheros locales; el botón HOME va a `index.html` (portada nueva en la raíz).
3. Eliminados: barra y scripts de Wayback, enlaces a webs externas, publicidad, contadores, rastreadores y los ficheros de `archivos/` que quedaron sin uso.
4. `*/src`: historia de versiones reconstruida, un commit por paquete de fuentes (ver README). El mensaje de cada commit lleva el nombre del paquete, su SHA-256 y la fecha del fichero más reciente.

## Cómo se hizo `src/` (para repetirlo o auditarlo)
- Extracción de cada paquete a una carpeta nueva y vacía; los `.tar.bz2` con el filtro `data` de `tarfile` de Python (rechaza rutas absolutas, `..` y enlaces que salgan de la carpeta) y los `.rar` con `node-unrar-js`, comprobando que ninguna ruta saliera de la carpeta destino.
- Para cada versión se borra su `src/` (en SDL+OpenGL solo la subcarpeta de ese paquete), se copia la nueva y se hace commit.
- Cada commit se verificó contra el paquete: `diff -r` del árbol commiteado con lo extraído. Todos idénticos. Los recuentos de ficheros coinciden con el listado de cada paquete.
- Diferencias con los paquetes: carpeta contenedora única quitada (NEO4ALL conserva `neo4all/`), 4 ficheros `.swp` omitidos, y las carpetas vacías (git no las guarda: 85 en FENIX, 4 en `nehe`, 1 en `kos-ports/SDL_gfx/lib`).
- Orden de NEO4ALL confirmado por las fechas internas: alpha one (2005-02) < alpha two < beta one < beta 2 (2005-04) < beta 3 (2005-10) < beta 4 (2005-11) < RC1 (2006-03) < RC3 fix (2007-04) < RC4 (2011-10). No existe RC2 en la web.
- Las fechas de los commits son las reales de creación (no se falsearon).

## Decisiones
- Los paquetes originales se conservan en `archivos/` porque las páginas enlazan a ellos.
- AES4ALL no tiene `src`: sus `.rar` son binarios y datos, sin código. Tampoco se extrajeron las versiones Windows/Dingoo ni los binarios (`1st_read.bin`).
- No se crea ninguna release de GitHub.
- No se añade licencia: se respetan las que traen los paquetes.

## Pendiente / ideas
- Si se quiere trabajar un emulador por separado, se puede sacar a su propio repositorio conservando la historia, p. ej. con `git filter-repo --subdirectory-filter NEO4ALL/src`.
- `NEO4ALL/src/neo4all` y `MAME4ALL/src` son la base para `mame4all-naomi` y otros ports a NAOMI; conviene citar a Chui en esos README.
- Hay 4 referencias `./archivos/blank` (sin extensión) en `UAE4ALL`, un fallo de la web original (espaciador de 2 px).
