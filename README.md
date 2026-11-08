<!-- Author: Matt Hejtman, login: HEJ0094 -->
# ZPG – Matt Hejtman (HEJ0094)

OpenGL 3.3 core, GLFW, GLAD (glad2, header-only), GLM. Build přes CMake. Podrobný popis kódu řádek po řádku je v [DOKUMENTACE.md](DOKUMENTACE.md).

## Struktura
```
CMakeLists.txt  cíle ZPG (aplikace) a composite_test (samostatný test kompozitu)
src/            Application, Shader, ShaderProgram, Model, DrawableObject, Scene, Camera,
                ICameraObserver, Light, Material, CustomMatrix, Transformation (abstraktní), Translate, Rotate, Scale,
                DynamicRotate, TransformationComposite, BasicModels.h, main.cpp
tests/          composite_test.cpp
assets/shaders/ model.vert, foliage.vert, clock.vert, lit.vert, litFoliage.vert, color.frag, normal.frag,
                constant.frag, lambert.frag, phong.frag, blinn.frag
assets/models/  přiložené modely (needitováno) + loginModel.h (převedený login),
                rocket.h (vlastní model rakety z Blenderu)
tools/          convert_login.py (login.h 9 floatů → loginModel.h 6 floatů)
docs/           class_diagram.png
libs/glad/      GLAD 2 (GL 3.3 core)
libs/glm/       GLM 1.0.1
libs/glfw/      (jen Windows) include/ + lib-vc2022/glfw3.lib
screenshots/    cv1-2/, cv3/, cv4/, cv5/, cv6/, cv7/
```

## Build
**Linux:** `sudo apt install libglfw3-dev`, potom
```
cmake -B build && cmake --build build
./build/ZPG
./build/composite_test   # samostatný test kompozitu
```
**Windows / Visual Studio:** prebuilt GLFW (64-bit) → `include/` a `lib-vc2022/` do `libs/glfw/`. Ve VS *File → Open → Folder*, spustit cíl `ZPG`. Všechny cesty jsou relativní k `${CMAKE_SOURCE_DIR}` (ekvivalent `$(SolutionDir)`).

## Ovládání (cvičení 7)
| Vstup | Akce |
|---|---|
| 1–9, 0 | scéna: 1 les, 2 hodiny, 3 sluneční soustava, 4 pořadí T·R / R·T, 5 modelová matice, 6 login, 7 čtyři koule, 8 normály + test w = 5, 9 odlesk na odvrácené straně, 0 noční les |
| N | převod normál: 0 bez transformace → 1 modelová matice → 2 normálová matice (výpis do konzole) |
| O | ořezání odlesku na odvrácené straně (Phong) zap/vyp |
| V | změna okna mění i aspect (správně) / jen viewport (roztažené) |
| C / L / P / B | osvětlení čtyř koulí: konstantní / Lambert / Phong / Blinn-Phong |
| F | další zorný úhel (30–170°) |
| W / S / A / D, pravé tlačítko myši | pohyb a rozhlížení kamery |
| ← → ↑ ↓, `+` / `−`, R | posun / zoom / otočení celé scény (i se světlem) |
| Esc | konec |

> České rozložení: `+` je na stejné fyzické klávese jako `1`, proto zoom používá numerickou klávesnici.

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
