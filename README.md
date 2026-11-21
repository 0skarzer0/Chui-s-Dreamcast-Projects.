# Chui-s-Dreamcast-Projects.
Chui's Dreamcast Projects.

Chui's Projects for Dreamcast and handhelds.
https://htmlpreview.github.io/?https://github.com/0skarzer0/Chui-s-Dreamcast-Projects./blob/main/index.html



	NEO4ALL : Open source NEOGEO/CD emulator for Dreamcast.
	AES4ALL : NEO4ALL fork for emulating home and arcade NEOGEO systems.
	DCASTAWAY : Portable Atari ST emulator.
	UAE4ALL : Open source Amiga500 emulator.
	SNES4ALL : Snes9x port (Super Nintendo emulator) for Dreamcast.
	ZX4ALL : Open source ZX-Spectrum emulator.
	MAME4ALL : MAME GP2X adaptation for Dreamcast and Dingoo.
	PCSX4ALL : Open source Playstation emulator.
	NOIZ2SA : Abstract shoot'em up for Dreamcast and GP32.
	FENIX : The way to game development for Dreamcast and GP32.
	SDL+OpenGL : Dreamcast developing with SDL and OpenGL.

## Código fuente descomprimido (`<PROYECTO>/src`)

Además de los paquetes originales (`<PROYECTO>/archivos/*.tar.bz2` y `*.rar`, que se
conservan porque las páginas enlazan a ellos), el código fuente de cada proyecto está
descomprimido en `<PROYECTO>/src`, para poder modificarlo y hacer commits normales.

La historia de versiones está reconstruida: **un commit por paquete de fuentes, en orden
cronológico** (por la fecha de los ficheros de cada paquete). Los commits llevan la fecha en
que se crearon en este repositorio, no la de 2004-2011. Cada mensaje de commit indica el paquete
original, su SHA-256 y la fecha de su fichero más reciente.

| Proyecto | Carpeta | Versiones (un commit cada una) |
|---|---|---|
| NEO4ALL | `NEO4ALL/src/neo4all`, `NEO4ALL/src/convertneo` (esta última desde RC3) | alpha one, alpha two, beta one, beta 2, beta 3, beta 4, RC1, RC3 (fix), RC4 |
| DCASTAWAY | `DCASTAWAY/src` | RC7 |
| FENIX | `FENIX/src` | 0.84b (CVS 08-03-2008) |
| MAME4ALL | `MAME4ALL/src` | beta 2 |
| NOIZ2SA | `NOIZ2SA/src` | Dreamcast alpha, beta one |
| SDL+OpenGL | `SDL_OPENGL/src/examples`, `.../nehe`, `.../kos-ports` | tres paquetes distintos, un commit cada uno |
| SNES4ALL | `SNES4ALL/src` | beta 1, beta 2 |
| UAE4ALL | `UAE4ALL/src` | RC3 |
| ZX4ALL | `ZX4ALL/src` | beta 2 |
| PCSX4ALL | `pcsx4all/src` | 2 |

AES4ALL no tiene `src` propio porque **comparte código con NEO4ALL**: es el mismo programa compilado en
modo AES (AES/MVS en lugar de NEO·GEO CD). Su página lo dice («Download source code from main NEO4ALL
project») y solo ofrecía binarios y datos. El código del modo AES está en `NEO4ALL/src/neo4all/src/aes/`
desde la versión RC1 (con `NEO4ALL/src/convertneo/src/neoaes.c`, el conversor de ROMs a `.aes`); en
`NEO4ALL/src/neo4all/config.mk` se activa con la variable `AES` (línea `#AES=1` comentada), que da el
ejecutable `aes4all`. No se ha compilado para comprobarlo.
Tampoco hay fuentes de las versiones para Windows, Dingoo ni de los binarios (`1st_read.bin`).

Para ver la historia de un proyecto: `git log --stat -- NEO4ALL/src`, y para comparar dos
versiones: `git diff <commit-beta3> <commit-beta4> -- NEO4ALL/src`.

Qué se ha tocado respecto a los paquetes (nada más): se quita la carpeta contenedora cuando el
paquete tenía una sola (en NEO4ALL se conserva `neo4all/`), se omiten 4 ficheros temporales de
editor (`.swp`, en DCASTAWAY, MAME4ALL, SNES4ALL beta 1 y kos-ports) y **git no guarda carpetas
vacías**: no están las 85 de FENIX, las 4 de `nehe` y `kos-ports/SDL_gfx/lib`. Si un `make` las
necesita, hay que crearlas (`mkdir -p`).

## Créditos y licencias

- **Chui**: autor de todos estos proyectos y de la web original (`chui.dcemu.co.uk`).
- **Internet Archive (Wayback Machine)**: de donde se recuperó la copia de las páginas.
- Estos emuladores y juegos parten de trabajo de terceros (Snes9x, UAE, MAME, Noiz2sa,
  KallistiOS, SDL y otras bibliotecas incluidas en los paquetes). Los avisos de licencia que
  traen los propios paquetes (`COPYING`, `LICENSE`, `gpl.txt`, cabeceras de los ficheros) se
  conservan sin tocar dentro de cada `src`. Este repositorio no añade ninguna licencia nueva.
- Este repositorio es un archivo de preservación; el trabajo original es de sus autores.
