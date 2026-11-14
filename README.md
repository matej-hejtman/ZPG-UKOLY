<!-- Author: Matt Hejtman, login: HEJ0094 -->
# ZPG – Matt Hejtman (HEJ0094)

OpenGL 3.3 core, GLFW, GLAD (glad2, header-only), GLM. Build přes CMake. Podrobný popis kódu řádek po řádku je v [DOKUMENTACE.md](DOKUMENTACE.md).

## Struktura
```
CMakeLists.txt  cíle ZPG (aplikace) a composite_test (samostatný test kompozitu)
src/            Application, Shader, ShaderProgram, Model, DrawableObject, Scene, Camera,
                ICameraObserver, Light, Material, CustomMatrix, Transformation (abstraktní), Translate, Rotate, Scale,
                DynamicRotate, LineMove, GrowScale, TransformationComposite, Texture, TextureManager, CubeMap, Skybox, ModelLoader,
                BasicModels.h, main.cpp
tests/          composite_test.cpp
assets/shaders/ model.vert, foliage.vert, clock.vert, lit.vert, litFoliage.vert, color.frag, normal.frag,
                constant.frag, lambert.frag, phong.frag, blinn.frag, skybox.vert, skybox.frag
assets/models/  přiložené modely (needitováno) + login.h (login z Blenderu s barvou, podpis v rohu),
                loginModel.h (převedený login bez barvy, scéna 6),
                rocket.h (vlastní model rakety z Blenderu)
assets/shrek/   Shrek, Fiona, toaleta (OBJ + MTL + textury)
assets/skybox/  cubemap/ = 6 obrázků oblohy (posx … negz)
assets/EB408/   EB408.blend (z LMS), eb408.obj + MTL + textury (sestavená učebna)
assets/teren/   teren.obj + MTL + grass.png (terén z LMS, teren.zip)
assets/textures/ whiteboard_zpg.png (vlastní obsah tabule)
assets/test/    testovací textury (šachovnice, dřevo, náhradní skybox)
assets/Texture/ wooden_fence.png (podlaha ve scéně 12) + vzorový projekt k texturám
tools/          convert_login.py (login.h 9 floatů → loginModel.h 6 floatů),
                build_eb408.py (Blender: sestavení a export učebny)
docs/           class_diagram.png
libs/glad/      GLAD 2 (GL 3.3 core)
libs/glm/       GLM 1.0.1
libs/stb/       stb_image.h
libs/glfw/      GLFW 3.4 (zdrojáky, kompiluje se s projektem)
libs/assimp/    Assimp 5.4.3 (zdrojáky bez testů, kompiluje se s projektem, jen OBJ import)
.vscode/        nastavení pro VS Code (CMake Tools, spuštění F5)
screenshots/    cv1-2/, cv3/, cv4/, cv5/, cv6/, cv7/, cv8/, cv9/
```

## Build
Všechny knihovny jsou v `libs/` jako zdrojáky a CMake je zkompiluje spolu s projektem.
**Nic se nestahuje ani neinstaluje.** Potřeba je jen kompilátor C++ (MSVC / GCC / Clang) a CMake ≥ 3.16.
První build trvá kvůli Assimpu chvilku déle, další už jsou rychlé.

**VS Code (Windows i Linux):**
1. Otevřít složku projektu (*File → Open Folder*).
2. Nainstalovat doporučená rozšíření, která VS Code nabídne (C/C++ a CMake Tools).
3. Při dotazu na kit vybrat kompilátor (na Windows *Visual Studio … amd64*).
4. **F5** = build + spuštění v debuggeru (konfigurace *ZPG (Windows / MSVC)* nebo *ZPG (Linux / GDB)*),
   nebo tlačítko ▶ ve spodní liště CMake Tools.

**Visual Studio:** *File → Open → Folder*, spustit cíl `ZPG`.

**Příkazová řádka:**
```
cmake -B build && cmake --build build
./build/ZPG
```
Všechny cesty jsou relativní k `${CMAKE_SOURCE_DIR}`, složka `assets/` se po buildu kopíruje i vedle `.exe`.

## Ovládání (cvičení 9)
Všechna nastavení jsou na **F1–F11** a **I**, Shift = opačný směr / druhá akce. Původní písmena z dřívějších cvičení (v závorce) fungují dál.

| Klávesa | Nastavení |
|---|---|
| F1 | nápověda: vypíše ovládání do konzole |
| F2 / Shift+F2 | režim levého tlačítka: select → insert → path / zpět (M) |
| F3 / Shift+F3 | osvětlení čtyř koulí ve scéně 7: konstantní → Lambert → Phong → Blinn-Phong / zpět (C / L / P / B přímo) |
| F4 / Shift+F4 | zorný úhel 30–170°: větší / menší (F) |
| F5 / Shift+F5 | převod normál: 0 bez transformace → 1 modelová matice → 2 normálová matice / zpět (N) |
| F6 | ořezání odlesku na odvrácené straně (Phong) zap/vyp (O) |
| F7 | změna okna mění i aspect (správně) / jen viewport (roztažené) (V) |
| F8 | UV test: texturovací souřadnice jako barva (červená = u, zelená = v) (U) |
| F9 / Shift+F9 | další / předchozí scéna (TAB / Shift+TAB) |
| F10 | vrátit posun / otočení / zoom scény do výchozího stavu |
| Shift+F10 | smazat objekty vložené kliknutím v aktivní scéně (stromy, Shrekové, koule) |
| F11 | celá obrazovka zap/vyp |
| I | vypíše aktuální nastavení (dříve F12 – na Windows F12 s debuggerem zastaví program) (scéna, režim, vložené objekty, sebrané mince, FOV, normály…) |

| Vstup | Akce |
|---|---|
| 1–9, 0 | scéna: 1 les, 2 hodiny, 3 sluneční soustava, 4 pořadí T·R / R·T, 5 modelová matice, 6 login, 7 čtyři koule, 8 normály + test w = 5, 9 odlesk na odvrácené straně, 0 noční les |
| F9 / TAB | i scény bez číselné klávesy: 11 Shrek, 12 Shrek + Fiona + toaleta, 13 učebna EB408, 14 terén |
| levé tlačítko myši | podle režimu: výběr objektu / vložení modelu na zem / bod trasy (výpis pixelu a unProject do konzole) |
| W / S / A / D, pravé tlačítko myši | pohyb a rozhlížení kamery |
| E / Q | kamera nahoru / dolů (po svislé ose y) |
| Shift + W / S / A / D / E / Q | kamera 3× rychleji |
| ← → ↑ ↓, `+` / `−`, R | posun / zoom / otočení celé scény (i se světlem) |
| Esc | konec |

> České rozložení: `+` je na stejné fyzické klávese jako `1`, proto zoom používá numerickou klávesnici.

## Souhrn – cvičení 9
Screenshoty jsou v `screenshots/cv9/` (pod obrazem je výpis z konzole k danému úkolu). Picking funguje ve všech scénách, vkládání v lese (1), nočním lese (0), učebně (13) a na terénu z LMS (14).

| Úkol | Stav | Řešení v kódu | Teorie |
|---|---|---|---|
| 1a | Splněno | `DrawableObject` má `id` (0–255) a druh `Kind { Normal, Ground, Collectible, Moving }`. `Scene::add` přidělí další volné ID (od 1, uvolněná ID z `remove` se použijí znovu, nad 255 → 0 a varování). V `draw()` těsně před `model->draw()` `glStencilFunc(GL_ALWAYS, id, 0xFF)`, skybox a podpis mají 0. Okno `GLFW_STENCIL_BITS 8`, `glEnable(GL_STENCIL_TEST)`, `glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE)`, `glClear` maže i stencil. `HEJ0094_1a.png` | Stencil buffer drží pro každý pixel 8bitové číslo. `GL_REPLACE` jen při třetím případu (prošel stencil i hloubka), takže v pixelu zůstane ID objektu, který je opravdu vidět. 0 = pozadí, proto nejvýš 255 objektů. |
| 1b | Splněno | `glfwSetMouseButtonCallback`: levé tlačítko jen uloží pozici. V `run()` po vykreslení a **před** `glfwSwapBuffers` `handleClick()`: přepočet na pixely framebufferu, `newy = h − 1 − y`, 3× `glReadPixels` (RGBA, `GL_DEPTH_COMPONENT`, `GL_STENCIL_INDEX`), `scene->getObjectById` a zvýraznění `objectColor × (1; 0,4; 0,4)`. `HEJ0094_1b.png` | GLFW má počátek kurzoru vlevo nahoře, framebuffer vlevo dole. Po prohození bufferů je obsah zadního bufferu nedefinovaný, proto se čte ve stejném snímku. |
| 2a | Splněno | Při hloubce < 1 `glm::unProject(vec3(x, newy, depth), view, projection, vec4(0, 0, w, h))`, výpis `unProject [x, y, z]`. Klik na zem dává y ≈ 0. `HEJ0094_2a.png` | unProject jde řetězcem pozpátku: okno → NDC (×2 − 1) → (P·V)⁻¹ → dělení w. Hloubka 1 = nic pod kurzorem (obloha), bod neexistuje. |
| 2b | Splněno | Režim insert (M): klik na objekt druhu `Ground` vloží strom: kompozit **bez** `base` scény, `Translate(pos)` + náhodná `Rotate` kolem y + `GrowScale` (strom za 2 s vyroste z 0). V učebně Shrek přes `loadObject(..., useBase = false)`. Na terénu (scéna 14, `teren.obj` z LMS, `Ground`) stromy stojí na svazích v různé výšce. `HEJ0094_2b.png`, `HEJ0094_2b_teren.png` | Bod z unProject je ve světových souřadnicích. Se sdíleným `base` by se po posunu scény šipkami objekt objevil jinde, než kam se kliklo. |
| 3a | Splněno | Režim path: 1. klik = A, 2. klik = B. List `LineMove(A, B, 2 m/s)`: `s = v·(čas − start)/|B − A|`, `t = 1 − |1 − (s mod 2)|`, `X = (1 − t)·A + t·B`. Oranžová koule [LineMove, Scale 0,3], druh `Moving`. `HEJ0094_3a.png` | Parametrická úsečka X(t) = A + (B − A)·t, t ∈ ⟨0, 1⟩: t = 0 → A, 0,5 → střed, 1 → B. Dělení délkou úsečky převádí rychlost v jednotkách za sekundu na rychlost parametru, takže krátká i dlouhá trasa se jede stejně rychle. |
| 3b | Splněno | V režimu select klik na `Moving` → `scene->remove`, výpis „Pohybujici se objekt X odstranen“. Navíc 10 zlatých koulí `Collectible` v lese, po kliknutí zmizí („Sebrano k / 10“). `HEJ0094_3b.png` | ID se čte ze snímku, který je právě vykreslený, takže sedí i na objekt, který se pohybuje. |
| 3c | Splněno | `enum class InteractionMode { Select, Insert, Path }`, F2 (Shift+F2 zpět) nebo M dokola, výpis `Mode: ...`. Přepnutí režimu zahodí rozpracovaný bod A, přepnutí scény i výběr. `HEJ0094_3c.png` | |
| 3d | Splněno | Les (scéna 1, zem `Ground`) i učebna EB408 (scéna 13, místnost načtená jako `Ground`, takže se dá vkládat na podlahu i stoly). `HEJ0094_2b.png`, `HEJ0094_3d.png` (Shrekové na podlaze a na stole, koule na trase) | |

## Souhrn – cvičení 8
Screenshoty jsou v `screenshots/cv8/`.

| Úkol | Stav | Řešení v kódu | Teorie |
|---|---|---|---|
| 1a | Splněno | Knihovna stb_image (`libs/stb`, `STB_IMAGE_IMPLEMENTATION` jen v `Texture.cpp`). Třída `Texture`: `stbi_set_flip_vertically_on_load(true)`, `stbi_load(..., 4)`, `glTexImage2D` RGBA, `glGenerateMipmap`, `GL_REPEAT`, `GL_LINEAR_MIPMAP_LINEAR` / `GL_LINEAR`. Shrek z OBJ přes Assimp (`ModelLoader`, 8 floatů na vrchol, UV = atribut 2). `phong.frag`: `texture(textureUnit, texCoord)` jako základní barva Phonga. Scéna 11 (TAB). `HEJ0094_1a.png`, UV test klávesou U `HEJ0094_1a_uv_test.png` | Obrázky mají počátek vlevo nahoře, OpenGL vlevo dole, proto flip. Mipmapy = zmenšené kopie textury. Trilineární filtrování míchá bilineární výsledek ze dvou sousedních úrovní, takže vzdálená textura nebliká. |
| 1b | Splněno | Každý `DrawableObject` drží vlastní `Texture*`. Podlaha je `texturedFloor` z `BasicModels.h` s opakovanou texturou dřevěných prken `assets/Texture/wooden_fence.png`. V `draw()` ji naváže na jednotku 0 (`glActiveTexture` + `glBindTexture`) a pošle `textureUnit = 0`, `hasTexture = 1`. `TextureManager` načte každý obrázek jen jednou. Scéna 12: Shrek, Fiona a toaleta, každý s texturou ze svého MTL. `HEJ0094_1b.png` | Sampler odkazuje na texturovací jednotku (slot), ne na ID textury. Mezi draw cally se do stejného slotu naváže jiná textura, proto stačí jedna jednotka pro všechny objekty. |
| 1c | Splněno | `CubeMap`: 6 obrázků `posx, negx, posy, negy, posz, negz` do `GL_TEXTURE_CUBE_MAP_POSITIVE_X + i`, bez flipu, `GL_CLAMP_TO_EDGE` v S, T, R, `GL_TEXTURE_CUBE_MAP_SEAMLESS`. `Skybox`: krychle `skycube` (3 floaty na vrchol), `skybox.vert` s `mat4(mat3(viewMatrix))`, kreslí se v `Scene::draw` první s `glDepthMask(GL_FALSE)`. `HEJ0094_1c.png` | Cubemapa se čte směrovým vektorem (pozice na krychli = směr ze středu). Odstraněním posunu z view matice zůstává krychle kolem kamery, takže obloha nejde dohnat. Bez zápisu hloubky ji každý objekt překreslí. |
| 2a | Splněno | Skript `tools/build_eb408.py` (Blender 4.5): do `EB408.blend` připojí (append) dveře a tabuli z LMS a umístí je podle stěn změřených paprsky. Export do `assets/EB408/eb408.obj` (Forward −Z, Up Y, UV, normály, materiály, triangulace, kopie textur). Assimp z něj udělá 7 částí s vlastní texturou. Scéna 13: světlo pod stropem, ambient 0,3, kamera uvnitř, skybox za okny. `HEJ0094_2a.png` | Blender má osu Z nahoru, OpenGL Y. Exportér přepočítá (x, y, z) → (x, z, −y). Každý materiál OBJ = jedna `aiMesh` = jeden `DrawableObject`. |
| 2b | Splněno | Přidány modely z LMS: katedra v pravém předním rohu naproti dveřím, 9 stolů ve třech řadách, 19 oranžových židlí čelem k tabuli (v Blenderu) a Shrek u katedry (v kódu přes `loadObject`, textura sdílená přes `TextureManager`). `HEJ0094_2b.png` | Kopie v Blenderu jsou linked duplicates (sdílený mesh, vlastní transformace). Shrek je samostatný objekt se sdíleným modelem a texturou. |
| 2c | Splněno | Tabule má vlastní obrázek `assets/textures/whiteboard_zpg.png` s Phongovým modelem (vzorce I = I_a + I_d + I_s, složky, reflect, útlum, obrázek vektorů n, l, r, c). Skript dá jen popisovatelné ploše nový materiál a rovinné UV, rám si nechá původní texturu. `HEJ0094_2c.png` | Původní textura tabule je atlas (rám + plocha). Proto se nemění celý obrázek, ale jen plochy desky dostanou nový materiál. |
| 2d | Čeká | Video 30–60 s průchodu učebnou (OBS nebo Win + G) natočí a nahraje autor na Discord. Odkaz doplnit sem. | |

## Souhrn – cvičení 7
| Úkol | Stav | Řešení v kódu | Teorie |
|---|---|---|---|
| 1a | Splněno | `lit.vert`: `uniform int normalMode`, klávesa N. Režim 0 = normála bez transformace, 1 = `mat3(modelMatrix) * normal`. Scéna 8: koule `Rotate(30°, z)` · `Scale(2, 1, 1)`, Phong, světlo bez útlumu. `HEJ0094_1a.png` (režim 1), `HEJ0094_1a_mode0.png` | Bez transformace normála ignoruje rotaci i měřítko, takže světlo „sedí“ jako na neotočené kouli. S modelovou maticí se normála při nerovnoměrném měřítku natáhne stejně jako povrch, a proto přestane být kolmá na povrch (odlesk a hranice světla jsou posunuté). |
| 1b | Splněno | Režim 2: `transpose(inverse(mat3(modelMatrix))) * normal`. Výchozí pro všechny osvětlené programy. `HEJ0094_1b.png` | Tečna t se transformuje maticí M, pro normálu n hledáme N tak, aby (Nn)ᵀ(Mt) = nᵀNᵀMt = 0 pro každé t s nᵀt = 0. Stačí NᵀM = I, tedy **N = (M⁻¹)ᵀ**. Normála je směr, proto jen 3×3 část bez posunu. N = M platí jen pro rotace (M⁻¹ = Mᵀ), u uniformního měřítka až na délku. |
| 2a | Splněno | Scéna 9: světlo (0, 0, −4) za koulí, kamera (0, 0, 5), `Material.shininess = 1`. `phong.frag`: `uniform bool cutBackSpecular`, klávesa O vypnuto. `HEJ0094_2a.png` | Při mocnině 1 je odlesk max(c·r, 0) velmi široký. r se dá spočítat i pro plochy, kam světlo nedopadá (n·l < 0), a kamera ho vidí jako světlý okraj na tmavé straně. To je fyzikálně nesmysl. |
| 2b | Splněno | `if (cutBackSpecular && dot(n, l) < 0) specular = 0` (klávesa O zapnuto, výchozí). `HEJ0094_2b.png` | Zrcadlová složka má smysl jen tam, kam světlo dopadá, tedy n·l > 0. Jinak je nulová stejně jako difúzní. |
| 2c | Splněno | `bool correctResize`, klávesa V. Při false callback změny okna volá jen `glViewport`, aspect kamery zůstává starý. Okno 800 × 600 → 1000 × 450. `HEJ0094_2c.png` | Viewport roztáhne obraz na nové pixely, ale projekce pořád počítá s poměrem 4:3. Obraz se zdeformuje (koule jsou elipsy). Bez viewportu by se kreslilo jen do původní části okna. |
| 2d | Splněno | Při true callback volá `glViewport(0, 0, w, h)` i `camera->setAspect(w, h)` (Observer pošle novou projekci). `HEJ0094_2d.png` | Projekční matice `perspective(fov, w/h, near, far)` s aktuálním poměrem stran zachová tvary. Viewport + aspect musí jít vždy spolu. |
| 3a | Splněno | Scéna 0: les z cv. 5 (stejný generátor, seed 42), osvětlené stromy (`litFoliage.vert` + Phong), zem `plain.h`, pozadí `glClearColor(0.01, 0.01, 0.03)`. Třída **Material** (ambient, diffuse, specular, shininess), noční materiál: **ambient 0,02**, diffuse 1, specular 0,15, shininess 16. Třída **Light** s útlumem, světluška: **c = 1,0, l = 0,35, q = 0,44**, barva žlutozelená. Světluška = malá konstantní koule na kružnici o poloměru 5 (DynamicRotate). Pozice světla se každý snímek bere z jejího kompozitu (`Scene::setLightTransformation`). `HEJ0094_3a.png` | Útlum bodového světla att = 1 / (c + l·d + q·d²), oříznutý do ⟨0, 1⟩. Kvadratický člen odpovídá ubývání intenzity se čtvercem vzdálenosti. Při l = 0,35 a q = 0,44 je ve 3 jednotkách útlum 0,17 a v 5 jednotkách 0,07, takže světluška osvítí jen okolí. Útlum násobí difúzní a zrcadlovou složku, **ne ambient**: ambient je světlo okolí bez konkrétního zdroje, nemá vzdálenost. |

Navíc (testovací otázka z přednášky): ve scéně 8 je koule s modelovou maticí, která má **w = 5** vpravo dole (`CustomMatrix`). Po dělení w je 5× menší, stejně velká a stejně osvětlená jako vedlejší koule se `Scale(0.2)`, protože `lit.vert` počítá `worldPosition = wp.xyz / wp.w` a GPU dělí `gl_Position` w automaticky.

## Souhrn – cvičení 6
| Úkol | Stav | Popis |
|---|---|---|
| 1a | Splněno | `constant.frag`: výsledná barva = barva objektu (barva vrcholu × `objectColor`), žádné světlo ani normály. `HEJ0094_1a.png` |
| 1b | Splněno | `lambert.frag`: ambient (0,1 × barva) + difúzní složka `max(dot(n, l), 0)` × barva × barva světla (Lambertův kosinový zákon). `HEJ0094_1b.png` |
| 1c | Splněno | `phong.frag`: Lambert + zrcadlová složka `pow(max(dot(c, reflect(-l, n)), 0), shininess)`, shininess 32. Odlesk je nulový, když je světlo za plochou. `HEJ0094_1c.png` |
| 1d | Splněno | `blinn.frag`: zrcadlová složka přes half-vektor `h = normalize(l + c)`, `pow(max(dot(n, h), 0), shininess)`. Odlesk je při stejné shininess větší a měkčí než u Phonga. `HEJ0094_1d.png` |
| 2a | Splněno | Slunce kreslí `constantProgram` (barvy vrcholů, rovnoměrně jasné) a je zdrojem světla (světlo v jeho středu). `HEJ0094_2a.png` |
| 2b | Splněno | Země a Měsíc kreslí `phongProgram`: strana ke Slunci je osvětlená, s odleskem, odvrácená strana tmavá (jen ambient). `HEJ0094_2b.png` |
| 2c | Splněno | Přidaná **raketa (vlastní model z Blenderu, `assets/models/rocket.h`)**, formát pozice + normála + barva, `blinnProgram`. Vlastní kompozit: sdílí oběžnou dráhu Země a obíhá kolem Země (rychleji než Měsíc, opačným směrem, ve vzdálenosti 0,3). `HEJ0094_2c.png` |
| 3 | Splněno | Scéna 8: bodové světlo v [0, 0, 0] (označené malou bílou koulí), koule v [±2, 0, 0] a [0, ±2, 0], kamera v [0, 0, 8] se dívá do počátku. Klávesy C/L/P/B přepnou program všech čtyř koulí. Každá koule má světlou stranu i odlesk směrem ke středu. `HEJ0094_3.png` |
| 3a | Splněno | Callback změny okna volá `glViewport` i `camera->setAspect`. V úzkém vysokém okně 400 × 800 zůstávají koule kulaté. `HEJ0094_3a.png` |

## Souhrn – cvičení 5
| Úkol | Stav | Popis |
|---|---|---|
| 1 | Splněno | Třída `Camera`: pohledová matice `glm::lookAt(eye, eye + target, up)` a projekční `glm::perspective(60°, aspect, 0.1, 100)`. Vertex shadery násobí `projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0)`, view a projection mají výchozí hodnotu `mat4(1.0)`. Každá scéna má vlastní kameru. Při změně velikosti okna se mění viewport i aspect (screenshot `HEJ0094_resize_1000x450.png`: koule zůstávají kulaté). `HEJ0094_1.png` |
| 2 | Splněno | W/S/A/D posouvají kameru (`glfwGetKey`, rychlost × deltaTime), myš s drženým pravým tlačítkem otáčí úhly α, φ, kurzor se nezamyká. α je omezené na ⟨1°, 179°⟩, aby se kamera nepřeklopila. Les: zem 100 × 100, 80 stromů a 60 keřů (náhodná pozice, rotace, měřítko, seed 42, volné mýtiny u startu a u hodin). Kmeny hnědé a listy v odstínech zelené podle `gl_VertexID` (dřevo = prvních 36 444 vrcholů `tree.h`). `HEJ0094_2.png` |
| 3 | Splněno | Hodiny = jeden `Model` a jeden `DrawableObject`, tedy jedno `glDrawArrays`. Ručičky se rozliší barvou vrcholu (červená = vteřinová, zelená = minutová, modrá = hodinová) a `clock.vert` je otočí kolem středu ciferníku o úhly z uniformů. Úhly se počítají každý snímek z místního času (`std::time` + `localtime_s`/`localtime_r`): s·6°, m·6° + s·0,1°, h·30° + m·0,5°, ve směru hodinových ručiček záporně. Ověřeno vykreslením v 3:00:15 a 10:10:40. `HEJ0094_3.png` (15:21:54), `HEJ0094_3b_10-10-40.png` |
| 4 | Splněno | Troje hodiny v lese na různých místech, natočené ke startu kamery. Jen uniformní velikost. Ciferník modelu je z viditelné strany zrcadlově, proto `Scale(-s, s, s)` (tip z LMS): velikost je ve všech osách stejná, takže se nic nedeformuje. `HEJ0094_4.png` |
| 5 | Splněno | Observer: `ICameraObserver::onCameraChanged(Camera*)`, `Camera` (subjekt) drží `vector<ICameraObserver*>`, `attach()` a `notifyAll()` (volá se po každé změně: pohyb, otočení, aspect). `ShaderProgram` implementuje `ICameraObserver` a v `onCameraChanged` pošle `viewMatrix` a `projectionMatrix`, takže matice jdou na GPU jen při změně. `Camera.h` a `ShaderProgram.h` se navzájem neincludují (dopředná deklarace `class Camera;`). `HEJ0094_5.png` |

## Souhrn – cvičení 4
| Úkol | Stav | Popis |
|---|---|---|
| 1 | Splněno | `model.vert`: `gl_Position = modelMatrix * vec4(position, 1.0)`. Uniform `mat4 modelMatrix` nahradil tři uniformy z cv. 3. Pozice se rozšiřuje na `vec4` s w = 1, aby se projevil posun (4. sloupec matice). `HEJ0094_1.png` |
| 2 | Splněno | `DrawableObject` drží ukazatel `Transformation*` (agregace, nevlastní ho), obvykle kompozit. V `draw()` pošle `setUniform("modelMatrix", transformation->getMatrix())`. `HEJ0094_2.png` |
| 3 | Splněno | Statické `Translate`, `Rotate`, `Scale` a dynamická `DynamicRotate`, která úhel počítá v `getMatrix()` z celkového času `glfwGetTime()`. Dynamické jsou i ovládací transformace scény (mění je klávesy ve vykreslovací smyčce). `HEJ0094_3a.png` (t = 0,5 s), `HEJ0094_3b.png` (t = 2,5 s) |
| 4 | Splněno | Scéna 2, jedna sdílená `DynamicRotate(45°/s, z)` ve dvou kompozitech. **[Translate, Rotate] → M = T·R**: na vrchol se nejdřív aplikuje matice vpravo (rotace kolem vlastního těžiště), pak posun, takže se trojúhelník **točí na místě**. **[Rotate, Translate] → M = R·T**: vrchol se nejdřív posune o 0,5 a pak se otočí kolem počátku světa, takže trojúhelník **obíhá střed okna** po kružnici o poloměru 0,5. Násobení matic není komutativní. Odchylka od zadání: první trojúhelník má posun −0,5 místo 0,5, aby byly na začátku vedle sebe a nepřekrývaly se. `HEJ0094_4.png` |
| 5 | Splněno | Vzor Kompozit: abstraktní `Transformation` (Component), listy `Translate`/`Rotate`/`Scale`/`DynamicRotate`, `TransformationComposite` (Composite) násobí matice dětí v pořadí přidání (M = M · child). `addStatic()` statické transformace hned **slučuje do jedné matice** (S S S S D S D se uloží jako S D S D), `addDynamic()` ukládá ukazatel, takže se hodnota čte každý snímek a dítě lze sdílet. Otestováno nejdřív samostatně programem `composite_test` (všechny kontroly OK, výstup `screenshots/cv4/composite_test_output.txt`). `HEJ0094_5.png` = UML diagram |
| 6 | Splněno | Scéna 3: `std::mt19937` s pevným seedem 42 a `uniform_real_distribution`. 30 stromů a 30 keřů, každý má kompozit [Translate(x, 0, z), Rotate(úhel, y), Scale(s)]. Model stromu a keře je na GPU jen jednou, všechny objekty ho sdílejí. Scéna je nakloněná o 25° kolem osy x, aby byla vidět zem. `HEJ0094_6.png` |
| 7 | Splněno | Scéna 4: Slunce [DynamicRotate(10, y), Scale(0.3)]. Samostatný sdílený kompozit **earthOrbit** = [DynamicRotate(20, y), Translate(0.6, 0, 0)]. Země = [earthOrbit, DynamicRotate(90, y), Scale(0.1)]. Měsíc = [earthOrbit, DynamicRotate(60, y), Translate(0.2, 0, 0), Scale(0.04)]. Měsíc obsahuje **stejný objekt** oběžné transformace Země, takže je vždy tam, kde Země, a k tomu přidává vlastní oběh a vzdálenost, proto obíhá Zemi, zatímco Země obíhá Slunce. Vlastní rotace a měřítko Země jsou jen v kompozitu Země, do Měsíce se nepropisují. `HEJ0094_7.png` |

## Souhrn – cvičení 3
| Úkol | Stav | Popis |
|---|---|---|
| 1 | Splněno | Posun, měřítko a rotace kolem osy y počítané rovnicemi v `transform.vert` (pořadí měřítko → rotace → posun). Hodnoty jdou jako uniformy `translation`, `scale`, `angle` z třídy `Transformation`. Ovládání šipkami, +/− a R. `HEJ0094_1.png` |
| 2 | Splněno | Přetížené `ShaderProgram::setUniform` pro `int`, `float`, `glm::vec3`, `glm::mat4`. Při `glGetUniformLocation == -1` se vypíše `Uniform <name> not found` (jednou za program) a nic se nenastaví. `HEJ0094_2.png` |
| 3 | Splněno | `Scene` drží objekty, `Application` drží seznam scén a přepíná je klávesami 1–9. `HEJ0094_3.png`, `HEJ0094_4.png` |
| 4 | Splněno | Testovací scény: trojúhelník, koule, les, login. |
| 5 | Splněno | Les: zem, 13 stromů, 13 keřů (každý jiná pozice, měřítko i natočení) a koule jako slunce. `HEJ0094_5.png` |
| 6 | Splněno | `main` jen vytvoří `Application` a zavolá `initialization()`, `createShaders()`, `createModels()`, `run()`. `HEJ0094_6.png` |
| 7 | Splněno | 3D text „HEJ0094“ vytvořen v Blenderu (`login.h`), převeden skriptem na formát pozice + normála (`loginModel.h`). Velký ve scéně 4, malý v pravém dolním rohu scén 1–3 jako podpis. `HEJ0094_7.png` |
| 8 | Splněno | Třídní diagram `docs/class_diagram.png`. `HEJ0094_8.png` |

## Souhrn – cvičení 1 + 2
| Úkol | Stav | Popis |
|---|---|---|
| 1 | Splněno | GLAD + vertex/fragment shader, modely přes VBO+VAO, barevný trojúhelník. |
| 2 | Splněno | Chyba v shaderu → výpis souboru a `glGetShaderInfoLog`, program skončí. |
| 3 | Splněno | Čtverec 6 vrcholů, 4. roh žlutý. |
| 4 | Splněno | Koule ze `sphere.h`, normála jako barva. |
| 5 | Splněno | Více modelů s více shader programy najednou. |
| 6 | Splněno | Relativní cesty, ověřeno zkopírováním projektu jinam. |
