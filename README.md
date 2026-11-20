# Transformation

    Transformation je abstraktni rozhrani. Definuje jediny kontrak: Kazda trida, ketra umi popsat heometrickou transformaci, musi umet vratit matici 4x4. Samotna trida nic nevypocitava, nic neuklada a nic nevlastni. Slouzi jako spolecny typ, presk ktery engine pracuje s libovolnou transformaci (posunm rotacem skala, vl;astni matice, slozena transformace, HUD umisteni, atd...) jednotne.

## Popis implementace

    - Transformation nema rodice, takze nic nededi.
    - Mame zde virtyalni destruktor, ktery je nastaven na default. Objekty se totiz budou ukladat pres ukazatel na zakladni tridu Transformation*. Bez virtualniho destruktoru by se pri delete pres ukazatel nezavolal destruktor potomka.
    - virtual glm::mat4 getMatrix() slouzi jako vzor pro ostatni tridy ktere budou z Transformation dedit. Kazda trida tedy bude muset nastavit getMatrix(), ale zalezi jen na ni jak to pojme.
    - implementuje se dale v CustomMatrix, translate, Rotate, Scale, HudPlacement, DynamicRotate, TransformationComposite a dalsich.

# ICameraObserver

    ICameraObserver je abstraktni rozhrani pro navrhovy vzor Observer. Trida, ktera chce vedet, ze se kamera zmenila,z nej dedi a implementuje jedinou metodu onCameraChanged(). Kamera drzi seznam pozorovatelu a pri kazde zmene je obejde a zavole jejich metodu. Samotne rozhrani nic nevypocitaba, nic neuklada a nic nevlastni.
    Hlavnim konkretnim pozorovatelem je ShaderProgram, ketry pri zmenie kamery nahraje doi shaderu matice viewMatrix a projectionMatrix a pripadne cameraPosition.

## Popis implementace

    - Stejne jako Transformation zde mame virtuali default destruktor. Kamera drzi pozorovatele jako ICameraObserver*. Pri mazani pres ukazatel na zakladni tridu by bez virtualniho destruktoru nebyl zavolan destruktor potomka.
    - virtual void onCameraChanged(Camera* camera) slouzi ke spracovani oznameni. Parametr je ukazatel a ne kopie, aby pozorovatel videl aktualni stav kamery a nevytvarel zbytecnou kopii. Metoda nevraci nic a slouzi jen ke spracovani oznameni, to je jen jednosmerne.

## Vztah k ostatnim tridam

    - Camera: ta si drzi std::vector<ICameraObserver*> observers. Metoda attach(ICameraObserver*) prida pozorovatele do seznamu a notifyAll() zavola onCameraChanged(this) u kazdeho z nich. NotifyAll() se vola pri zmene pozice (setPosition(), rotate(), moveForward(), moveDown()), pri zmene Fovm pomeru stran, vzdalenosti dalsiho rezu a dalsich parametrech.
    - ShaderProgram: Dedi z ICameraObserver. Metoda onCameraChanged zavola use(), nahraje viewMatrix a projectionMatrix a pokud shader obsahuje uniform cameraPosition, nahraje i pozici okna.
    - Scene: Pri registraci programu zavola camera.attach(program) a hned pote program->oncxamerachanged(&camera). Tim se shader ihned synchronizuje s aktualnim stavem kamery, i kdyz se kamera jeste nehybe.

## Na co si dat pozor

    - Kamera nema metodu detach. V tomto projektu by jsem takovou funkci nevyuzil. Pokud je ShaderProgram znicen drive, nez kamera, zustane vv seznamu visicich ukazatelu a notifyAll() do nej povede k nedefinovanemu chovani.
    - NotifyAll() je verejne, takze ji muzeme zavolad odkudkoli a vsichni pozorovatele dostanou oznameni i bez skutecne zmeny kamery, to se muze naopak zase nekdy hodit.

# SceneClock

    SceneClock je herni/scenovy cas. Na rozdil od glfwGetTime() nebo od realneho casu, ktery prichazi z venku z hlavni smycky nese vlastni cas sceny, ktery lze zpomalit zrychlit nebo i zastavit. Animace, ktera maji reagovat na cas sceny (rotace, pulzovani nebo pohyb kyvadel), se tak daji ovladat jednnou tridou bez zasahu do ostatnich casti enginu. Trida opet nic nevlastni a nema vztah k OpenLg. Je to cista logika casu.

## Popis implementace

    - privatni promene: time vyjadruje celkovy cas sceny, vychozi je nula. delta je posledni prirustekm vychozi nula dokud neni zavolan advance. Speed ma default hodnotu 1 neboli 1x zrychleno. a paused ma zakladni hodnotu false, jelikoz na zacatku cas neni zastaveny.

    - void advance(float ralDeltaTime); - toto je jedina metoda ktera doopravdy upravuje. Posune cas o realny prirustej vynasobeny rychlosti. Vola se jednou za snimek z hlavni smycky.
    - double getTime() const; - vraci time, tedy celkovy cass sceny
    - float getDelta() const; - vraci deltaTime, neboli prirustek casu v poslednim snimku
    - void setPaused(bool value); - nastavi pozastaveni
    - bool isPaused() const; - vraci jestli je cas pozastaven
    - void setSpeed(float value); - nastavuje rychlost plynuti casu
    - bool getSpeed() const; - vraci rychlost

## Navrhove rozhodnuti

    SceneClock je jediny zdroj casu, vsechny animace ctou cas z jednoho objektu, zmena nebo pauza se tak projevi vsude najednou.
    SceneClock je oddeleny od realneho a scenoveho casu, advice dostava realny prirustek a sam ho prepocitava, hlavni smycka tak nemusi znat pravidla sceny.
    Je to naprosto nezavisle, Trida nezna scenu, kameru ani shadery, diky tomuu je jednoduse testovatelna.

## Vztahy k tridam

    DynamicRotate - Drzi const SceneClock& a z nej pocita uhel jako startAngle + degPerSecond * (clock.getTime() - startTime). Zmena rychlosti pres setSpeed ulozi zktualni uhel jako novy pocatecni, aby nedoslo ke skoku.
    GrowScale - Pracuje s casem z hodin podobnym zpusobem jako v DynamicRotate
    Scene - Vlastni instanci hodin a cola advance v kazdem snimku.
    sceny - Napriklad ShowcaseScene cte getTime() pro pohyb kyvadel a sroubovice, getSpeed() a isPaused pro indikatory rychlosti v HUD

# Material

    Material popisuje, jak povrch odrazi svetlo. Je to datova struktura s barevnymi koeficienty Phongova osvetlovaciho modelu (ambient, diffuse a specular), koeficientem lesku a volitelnou texturou. Umi si nahrat sve hodnoty do shadery a obsahuje sadu predpripravenych materialu.

    Narozdil od Transformation nebo ICameraObserver nejde o rozhrani. Materaial je hodnotovy typ, ktery se kopiruje a predava hodnotu.

## Popis implementace

    glm::vec3 ambient = glm::vec3(0.1f);    - ambientni odrazivost, ktere dopada vsude bezzdroje, mala hodnota zajisti, ze neosvetlene strany nejsou uplne cerne, zakladni je seda
    glm::vec3 diffuse = glm::vec3(1.0f);    - Difuzni odrazivost slozka, ktera zavisi na uhlu dopadu svetla je to hlavni barva povrchu, zakladni je bila
    glm::vec3 specular = glm::vec3(0.5f);   - Zrcaslova odrazivost, urcuje barvu a silu leskleho odlesku
    float shininess = 32.f;                 - koeficient lesku, vyssi hodnota znamena mensi a ostrejsi odlesk.
    Texture* texture = nullptr;             - Nevlastneny ukazatel na texturu, nullptr znamena BEZ TEXTURY

    void apply(ShaderProgram* program) const;   - Kazda vlastnost se nahravajen tehdy, pokud shader ma odpovidajici uniform, Stejny material tak funguje i s ruznymy shadery, i kdyz nektere neobsahuji vsechny slozky. std::max(shininessm 1.0f) zaruci, ze shinines bude vzy vice nez 1 nebo prave 1

    static Material make(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess)

# CustomMatrix

    CustomMatrix obsahuje hotovou 4x4 matici a zpristupnuje ji pomoci rozhrani Transformation. Je to wrappen na glm::mat4 tak, aby se dal vlozit do TransformationComposite nebo aby s nim mohlo byt zachazeno jako s Transformation.

# BasicModels

    BasicModels je jen seznam zakladnich tvaru se kteryma engine pracuje jako napriklad trojuhelnik, cube, skycube atd...

# Texture

    Texture je RAII obal nad OpenGl texturou. Nacte obrazek ze souboru, nahraje pixely do GPU, nastavy zpusob opakovani a filtrovani a pri zaniku objektu na GPU uvolni.

    Mame zde TextureOptions, kde si muzeme vybrat bud repeat nebo pouziti mipmaps. default cesta je assets/textures/Default/default.png ktery jsem delal sam.

    Konstruktor vyzaduje path a TextureOptions. Bind zase pripoji texturu na texturovou jednotku. Settery a gettery vraci hodnoty z porivate casti.

# TextureOptions

    TextureOptions je mala pomocna struktura definovana v Texture.h. Drzi nastaveni, se kterymi se ma textura nahrat do GPU. Nic nevlastni, nic nededi, je to cisty datovy obal (hodnotovy typ).

## Popis implementace

    - bool repeat = true; - rezim opakovani (wrap). true = GL_REPEAT, textura se mimo rozsah UV 0..1 opakuje (hodi se na podlahu, kde jsou UV 0..10). false = GL_CLAMP_TO_EDGE, mimo rozsah se roztahne krajni pixel.
    - bool mipmaps = true; - jestli se vygeneruji mipmapy. true = glGenerateMipmap + GL_LINEAR_MIPMAP_LINEAR (trilinearni filtrovani, textura v dalce nezrni). false = jen GL_LINEAR.
    - Obe hodnoty jsou na sobe nezavisle, muzou byt zapnute obe, zadna, nebo kazda zvlast.
    - Vychozi hodnoty odpovidaji nejcastejsimu pouziti, takze konstruktor Texture lze volat jen s cestou (TextureOptions() je vychozi parametr).

## Vztah k ostatnim tridam

    - Texture - konstruktor podle ni nastavuje glTexParameteri.
    - Resources - getTexture(path, options) sklada klic cache z cesty a voleb ("|clamp", "|nomip"), takze stejny obrazek s jinymi volbami je v pameti jako samostatna textura.
    - ShowcaseScene - vytvari "ostrou" variantu vychozi textury (repeat = false, mipmaps = false), kterou pouziva na sloupy brany.

## Navrhove rozhodnuti

    - Misto dvou bool parametru v konstruktoru (Texture(path, true, false) - necitelne) je jedna pojmenovana struktura. Pri rozsireni (napr. filtr nearest) staci pridat polozku a stary kod funguje dal.

# Shader

    Shader je RAII obal nad jednim OpenGL shader objektem (vertex nebo fragment). Nacte zdrojovy kod ze souboru, zkompiluje ho a pri zaniku objektu shader na GPU smaze. Sam o sobe se nikdy nekresli, slouzi jen jako docasny stavebni kamen pro ShaderProgram.

## Popis implementace

    - Shader(GLenum type, const std::string& path); - type je GL_VERTEX_SHADER nebo GL_FRAGMENT_SHADER, path je cesta k souboru.
    - ~Shader(); - zavola glDeleteShader(id).
    - Kopirovaci konstruktor a operator= jsou smazane (= delete), stejne jako u Texture. Dve kopie by drzely stejne id a druhy destruktor by mazal uz smazany shader.
    - GLuint getId() const; - vraci id, ktere ShaderProgram potrebuje pro glAttachShader.
    - GLuint id = 0; - handle shaderu v OpenGL.
    - static std::string readFile(const std::string& path) - pomocna funkce viditelna jen v Shader.cpp (static). Otevre soubor pres std::ifstream, cely obsah precte najednou pres file.rdbuf() do std::stringstream a vrati ho jako string. Kdyz soubor nejde otevrit, vypise chybu a ukonci program.
    - Konstruktor: precte zdrojak, ziska z nej const char* (glShaderSource chce pole C retezcu), vytvori shader glCreateShader(type), preda mu zdrojak (1 retezec, nullptr = retezec je ukonceny nulou) a zkompiluje ho glCompileShader.
    - Kontrola kompilace: glGetShaderiv(GL_COMPILE_STATUS). Pri chybe se zjisti delka logu (GL_INFO_LOG_LENGTH), vytvori se buffer std::vector<char> (alespon 1 znak, aby data() nebylo prazdne), log se nacte glGetShaderInfoLog a vypise se i s cestou k souboru. Pak std::exit, protoze bez shaderu nema smysl pokracovat.

## Vztah k ostatnim tridam

    - ShaderProgram - v konstruktoru vytvori dva Shader objekty jako lokalni promenne, pripoji je, slinkuje program a po skonceni konstruktoru se Shadery automaticky smazou (RAII). Hotovy program uz zdrojove shadery nepotrebuje.

## Na co si dat pozor

    - Shader se musi vytvaret az po inicializaci OpenGL kontextu a GLAD.
    - Chyba kompilace ukonci cely program (std::exit), destruktory ostatnich objektu se nezavolaji. Pro skolni projekt je to v poradku, chyba se hned ukaze v konzoli.

# ShaderProgram

    ShaderProgram je RAII obal nad slinkovanym OpenGL programem (vertex + fragment shader). Zapouzdruje aktivaci programu, nastavovani uniform promennych a napojeni na uniform blok svetel. Zaroven je to konkretni pozorovatel kamery (dedi z ICameraObserver), takze kdyz se kamera zmeni, program si sam nahraje novou view a projection matici.

## Popis implementace

    - class ShaderProgram : public ICameraObserver - dedeni rozhrani pro Observer.
    - ShaderProgram(vertexPath, fragmentPath) - description se nastavi na "vertex + fragment" (pouziva se jen ve vypisech chyb, aby bylo jasne, ktery program ma problem). Vytvori dva docasne Shader objekty, glCreateProgram, glAttachShader na oba a glLinkProgram. Pak kontrola GL_LINK_STATUS stejne jako u Shaderu (log + std::exit). Nakonec glDetachShader, aby se shader objekty po skonceni konstruktoru opravdu smazaly.
    - ~ShaderProgram() override - kdyz je tento program prave aktivni, vynuluje cache activeProgram, a pak glDeleteProgram.
    - Kopirovani zakazano (= delete), drzi GPU handle.
    - static GLuint activeProgram - jedna sdilena promenna pro vsechny programy. Pamatuje si, ktery program je prave nastaveny pres glUseProgram.
    - use() const - zavola glUseProgram jen kdyz program jeste neni aktivni. Usetri zbytecna volani do driveru (stejny program se pouziva pro desitky objektu za sebou).
    - static unbind() - glUseProgram(0) a vynulovani cache. Vola se na konci Scene::draw().
    - isActive() - porovna id s activeProgram.
    - lookupLocation(name) const - hleda lokaci uniformu v locationCache (std::unordered_map). Kdyz tam neni, zepta se glGetUniformLocation a vysledek ulozi (i -1, tedy i informaci "neexistuje"). Cache je mutable, aby sla plnit i z const metody hasUniform.
    - getUniformLocation(name) - pouzije lookupLocation. Kdyz uniform neexistuje, vypise varovani, ale jen jednou pro kazde jmeno (set missing, insert().second je true jen poprve). V debug buildu (#ifndef NDEBUG) navic hlida, jestli se nenastavuje uniform na neaktivnim programu. To je casta chyba, glUniform* totiz vzdy pise do prave aktivniho programu.
    - setUniform(...) - pretizeni pro int, float, vec3, vec4, mat3 a mat4. Kazda verze zjisti lokaci, pri -1 nic nedela a jinak zavola odpovidajici glUniform*. glm::value_ptr vraci ukazatel na floaty vektoru nebo matice. GL_FALSE u matic znamena "netransponovat" (glm uklada matice po sloupcich stejne jako OpenGL).
    - hasUniform(name) const - true, kdyz shader uniform opravdu obsahuje. Pouziva ho Material::apply a DrawableObject::draw, aby stejny kod fungoval s ruznymi shadery.
    - onCameraChanged(Camera*) override - aktivuje program a nahraje viewMatrix, projectionMatrix a pokud shader ma uniform cameraPosition, tak i pozici oka kamery (potrebuje ho Phong pro odlesky).
    - hasUniformBlock(blockName) / bindUniformBlock(blockName, bindingPoint) - zjisti index uniform bloku (glGetUniformBlockIndex) a napoji ho na binding point (glUniformBlockBinding). Tim program cte data z UBO svetel (LightBuffer), ktery je na stejnem binding pointu.

## Vztah k ostatnim tridam

    - Shader - docasne stavebni kameny v konstruktoru.
    - Camera - ShaderProgram se do ni registruje jako pozorovatel (pres Scene::registerProgram).
    - Resources - vlastni vsechny programy (unique_ptr) a po vytvoreni programu zavola bindUniformBlock("LightBuffer", LIGHT_BINDING_POINT), pokud ho shader ma.
    - Material, DrawableObject, Skybox, Scene - volaji use() a setUniform().

## Navrhove rozhodnuti

    - Observer - shader se o zmene kamery dozvi sam, nikdo mu nemusi kazdy snimek rucne posilat matice.
    - Cache lokaci - glGetUniformLocation je pomale (driver porovnava retezce), proto se vysledek pamatuje.
    - Cache aktivniho programu - mene volani glUseProgram.
    - Tolerance k chybejicim uniformum - program nespadne, jen jednou varuje.

## Na co si dat pozor

    - activeProgram je spravne jen kdyz nikdo jiny nevola glUseProgram primo. V projektu se to deje jen pres tuto tridu.
    - Programy jsou sdilene mezi scenami (Resources je cachuje). Kazda scena ma vlastni kameru, ktera do stejneho programu zapisuje. Proto Scene::activate() vola camera.notifyAll(), aby se po prepnuti sceny do programu nahraly matice spravne kamery.
    - HUD program se jako pozorovatel kamery zamerne neregistruje, jinak by mu kamera prepsala ortho projekci perspektivni.

# lit.vert

    Vertex shader pro vsechny osvetlene objekty (pouziva se s phong.frag i showcase.frag). Prevede vrchol z lokalnich souradnic modelu do svetovych souradnic (pro vypocet svetla) a do clip space (pro rasterizaci).

## Popis implementace

    - #version 330 core - GLSL odpovidajici OpenGL 3.3 core profile, ktery se vytvari v Application.
    - layout(location = 0..3) in - vstupni atributy: pozice, normala, UV a barva. Cisla lokaci odpovidaji konstantam VertexAttribute v Model.h. Kdyz model nektery atribut nema, Model::draw mu nastavi vychozi hodnotu (glVertexAttrib*).
    - uniform mat4 modelMatrix - matice objektu (z Transformation::getMatrix()), nastavuje ji DrawableObject.
    - uniform mat3 normalMatrix = mat3(1.0) - matice pro normaly (transpose(inverse(mat3(model)))). Bez ni by se normaly pri nerovnomernem skalovani zkreslily. Vychozi hodnota je jednotkova, kdyby ji nikdo nenastavil.
    - uniform mat4 viewMatrix, projectionMatrix - nastavuje je kamera pres ShaderProgram::onCameraChanged.
    - out worldPosition, worldNormal, vertexColor, texCoord - predava se do fragment shaderu (GPU je mezi vrcholy interpoluje).
    - wp = modelMatrix * vec4(position, 1.0) - pozice ve svete. w = 1, protoze jde o bod (posun se uplatni).
    - worldPosition = wp.xyz / wp.w - deleni w pro jistotu, kdyby matice nebyla afinni (napr. CustomMatrix).
    - worldNormal = normalMatrix * normal - normalizuje se az ve fragment shaderu, protoze interpolace delku stejne rozbije.
    - gl_Position = projectionMatrix * viewMatrix * wp - klasicky MVP retezec.

# phong.frag

    Fragment shader s Phongovym osvetlovacim modelem. Pro kazdy pixel spocita ambientni, difuzni a zrcadlovou slozku od vsech svetel ve scene. Podporuje bodova, smerova a reflektorova svetla, utlum se vzdalenosti, material a volitelnou texturu.

## Popis implementace

    - #define MAX_LIGHTS 64 - musi sedet s MAX_LIGHTS v LightBuffer.h, jinak by se neshodovala velikost UBO.
    - struct Material - stejne slozky jako C++ Material (ambient, diffuse, specular, shininess). Plni ho Material::apply pres "material.ambient" atd.
    - struct LightSource - 5x vec4, presne odpovida C++ LightData. Vse jsou vec4 kvuli pravidlum zarovnani std140 (vec3 se zarovnava na 16 bajtu a vznikaly by mezery).
        - position.xyz - pozice ve svete
        - color.rgb - barva uz vynasobena intenzitou
        - attenuation.xyz - konstantni, linearni a kvadraticky clen utlumu
        - direction.xyz - smer (smerove svetlo a reflektor)
        - params.x = typ (0 bod, 1 smer, 2 reflektor), params.y = cos vnitrniho uhlu, params.z = cos vnejsiho uhlu
    - layout(std140) uniform LightBuffer - uniform blok: lightInfo.x = pocet svetel, ambientLight.rgb = ambientni svetlo sceny, lights[] = pole svetel. Data posila LightBuffer::update jednim volanim pro vsechny shadery najednou.
    - uniform objectColor - barva objektu (DrawableObject::setColor), nasobi se se zakladni barvou.
    - uniform cameraPosition - pozice kamery pro vypocet odlesku.
    - uniform sampler2D textureUnit + int hasTexture - kdyz ma material texturu, bere se barva z textury, jinak barva vrcholu.
    - attenuation(light, d) - utlum 1 / (kc + kl*d + kq*d^2), oriznuty na 0..1 (aby svetlo blizko zdroje nebylo silnejsi nez 1).
    - specularPhong(n, l, c) - kdyz je svetlo za povrchem (dot(n, l) < 0), odlesk je 0 (jinak by se objevoval odlesk i na odvracene strane). Jinak r = reflect(-l, n) a vysledek je pow(max(dot(c, r), 0), shininess).
    - main():
        - baseColor = (textura nebo barva vrcholu) * objectColor
        - n = normalizovana normala, c = smer z povrchu ke kamere
        - result zacina ambientni slozkou: material.ambient * ambientLight * baseColor
        - smycka pres svetla (max MAX_LIGHTS): smerove svetlo ma l = -direction a zadny utlum. Bodove svetlo a reflektor maji l = smer ke svetlu a utlum podle vzdalenosti. Reflektor navic nasobi utlum smoothstep(outerCos, innerCos, spot), tedy mekky prechod mezi vnitrnim a vnejsim kuzelem.
        - pricte se difuzni slozka (max(dot(n, l), 0) - Lambertuv zakon) a zrcadlova slozka.
        - fragColor = vec4(result, 1.0)

## Na co si dat pozor

    - Poradi argumentu smoothstep je dulezite: prvni je cos vnejsiho uhlu, druhy cos vnitrniho. Kosinus s uhlem klesa, takze vnejsi kosinus je mensi.
    - MAX_LIGHTS je na trech mistech (LightBuffer.h, phong.frag, showcase.frag) a musi se menit vsude.

# skybox.vert

    Vertex shader pro skybox. Kresli krychli kolem kamery tak, aby se pri pohybu kamery nehybala (je "nekonecne daleko"), jen se otaci spolu s pohledem.

## Popis implementace

    - layout(location = 0) in vec3 position - skycube ma jen pozice (VertexLayout::position()).
    - viewMatrix, projectionMatrix - nastavuje je kamera, protoze Scene::setSkybox registruje program skyboxu jako pozorovatele.
    - texCoord3 = vec3(-position.x, position.y, position.z) - pozice vrcholu krychle slouzi primo jako smer pro cube mapu. Osa x je otocena, protoze cube mapy pouzivaji levotocivou konvenci a obraz by byl jinak zrcadlove prevraceny.
    - mat4(mat3(viewMatrix)) - z view matice se vezme jen rotace (horni 3x3), posun se zahodi. Proto se skybox s kamerou nepohybuje.

## Vztah k ostatnim tridam

    - Skybox::draw vypina zapis do hloubky, takze krychle o velikosti 1 nic nezakryje, i kdyz je blizko.

# skybox.frag

    Fragment shader skyboxu. Precte barvu z cube mapy ve smeru texCoord3.

## Popis implementace

    - uniform samplerCube skybox - cube mapa. Skybox::draw ji pripoji na jednotku 0 a nastavi uniform "skybox" na 0.
    - fragColor = texture(skybox, texCoord3) - GPU sama vybere stenu krychle podle smeru vektoru.

# hud.vert

    Vertex shader pro HUD (2D prvky pres obrazovku). Nepouziva kameru, jen modelMatrix (umisteni z HudPlacement) a ortograficke promitani.

## Popis implementace

    - in position (location 0) a color (location 3) - ostatni atributy HUD nepotrebuje.
    - uniform modelMatrix - z HudPlacement (kotva + offset + skala).
    - uniform projectionMatrix - ortho(-aspect, aspect, -1, 1), nastavuje ji Scene::draw kazdy snimek pred kreslenim HUD.
    - gl_Position = projectionMatrix * modelMatrix * vec4(position, 1.0) - zadna view matice, HUD je pevne na obrazovce.

# hud.frag

    Fragment shader HUD. Bez osvetleni, jen barva vrcholu vynasobena barvou objektu.

## Popis implementace

    - fragColor = vec4(vertexColor * objectColor, 1.0). objectColor nastavuje DrawableObject::setColor, takze HUD prvek lze libovolne obarvit (ShowcaseScene tim dela LED indikatory).
    - Krychle pouzita jako HUD ctverec nema barvu vrcholu, Model::draw proto doplni bilou (1,1,1) a vysledna barva je ciste objectColor.

# showcase.frag

    Rozsirena verze phong.frag pro ShowcaseScene. Ma stejny Phonguv model a navic pridava mlhu, rozptyl svetla v mlze (svetelne "koule" kolem lamp) a rim light (svetly okraj objektu proti pozadi).

## Popis implementace

    - Vse z phong.frag (Material, LightSource, LightBuffer, attenuation, specularPhong) je stejne.
    - Nove uniformy, ktere nastavuje ShowcaseScene::applyEnvironment podle prostredi: fogColor, fogDensity, fogScatter, rimColor, rimStrength. Vychozi hodnoty jsou 0 = efekt vypnuty.
    - inScatter(origin, direction, rayLength, lightPosition, lightColor) - aproximace, kolik svetla z bodoveho zdroje se rozptyli v mlze podel paprsku od kamery k pixelu. t0 = projekce svetla na paprsek, h = kolma vzdalenost svetla od paprsku (min 0.05, aby se nedelilo nulou). Integral 1 / (h^2 + t^2) podel paprsku ma analyticke reseni pres atan, takze neni potreba zadna smycka (ray marching).
    - main():
        - toCamera a distanceToCamera - vzdalenost se pouzije pro mlhu i rozptyl, max(.., 1e-4) chrani pred delenim nulou
        - ve smycce se pro bodova svetla (ne reflektory) pricita scatter, kdyz je fogScatter > 0
        - rim = pow(1 - dot(n, c), 3) - nejvetsi na okrajich objektu, kde normala miri kolmo na smer pohledu
        - fog = exp(-(density * vzdalenost)^2) - exponencialni kvadraticka mlha, mix(fogColor, result, fog) prolne barvu objektu do barvy mlhy
        - k vysledku se pricte scatter * fogScatter

## Navrhove rozhodnuti

    - Specialni efekty jsou v samostatnem shaderu, zakladni phong.frag zustal jednoduchy pro ostatni sceny.
    - Uniformy s vychozi hodnotou 0 - shader funguje, i kdyz je nikdo nenastavi.

# Light

    Light popisuje jedno svetlo ve scene: bodove, smerove nebo reflektor. Drzi jeho vlastnosti (pozice, barva, intenzita, utlum, smer, uhly kuzele) a vlastni transformaci, se kterou se muze pohybovat spolu se scenou nebo s jinym objektem. Do GPU se samo nenahrava, jen se prevede na LightData a to posle LightBuffer.

## Popis implementace

    - enum class LightType { Point = 0, Directional = 1, Spot = 2 } - cisla odpovidaji params.x ve shaderu. enum class je typove bezpecny (neda se omylem zamenit s int).
    - privatni promenne a vychozi hodnoty:
        - position = (0,0,0) - pozice v lokalnich souradnicich (pred transformaci)
        - color = bila, intensity = 1
        - constant = 1, linear = 0.09, quadratic = 0.032 - vychozi utlum (dosah zhruba 50 jednotek, bezne tabulkove hodnoty)
        - enabled = true
        - type = Point, direction = (0,-1,0) - dolu
        - cutOffDeg = 12.5, outerCutOffDeg = 17.5 - vnitrni a vnejsi uhel reflektoru
        - TransformationComposite transformation - vlastni transformace svetla (vlastni ji hodnotou)
    - settery a gettery pro vsechny vlastnosti. Zajimave jsou:
        - setIntensity - std::max(value, 0), zaporna intenzita nedava smysl
        - setDirection - nulovy vektor odmitne s varovanim (nesel by normalizovat), jinak ulozi normalizovany smer
        - setSpotAngles(inner, outer) - oba uhly orizne na 0..89 stupnu (MAX_SPOT_ANGLE, pri 90 by kuzel byl polorovina). Kdyz je vnejsi uhel mensi nez vnitrni, nastavi ho na vnitrni a vypise varovani. Nakonec zajisti rozdil aspon 0.1 stupne (MIN_SPOT_SOFTNESS), protoze smoothstep se stejnymi hranami by delil nulou.
        - setCutOff / setOuterCutOff - meni jen jeden uhel, ale vzdy pres setSpotAngles, takze kontroly plati vzdy
    - getTransformation() - vraci referenci na kompozit, aby do nej scena mohla pridavat transformace (napr. Scene::addLight pripoji base, dron pripoji svou LineMove).
    - toData() const - prevod na strukturu pro GPU:
        - matrix = transformation.getMatrix()
        - worldPosition = matrix * vec4(position, 1) - bod, posun se uplatni
        - worldDirection = matrix * vec4(direction, 0) - smer, w = 0, takze posun se neuplatni, jen rotace (a skala, proto se potom normalizuje; kdyby vysel nulovy, pouzije se puvodni smer)
        - color = color * intensity - intenzita se zapece do barvy, shader ji pak nemusi resit
        - attenuation = (constant, linear, quadratic, 0)
        - params = (typ, cos(vnitrni uhel), cos(vnejsi uhel), 0) - kosiny se pocitaji na CPU jednou, shader porovnava primo dot produkt s kosinem a nemusi pocitat acos

## Vztah k ostatnim tridam

    - LightBuffer - toData() vraci LightData definovanou v LightBuffer.h.
    - TransformationComposite - kazde svetlo ma svuj.
    - Scene - vlastni svetla (vector<unique_ptr<Light>>) a v draw() prevede vsechna zapnuta svetla na LightData.
    - ShowcaseScene - nastavuje svetla (slunce, prstenec, drony, baterka, reflektor...).

## Navrhove rozhodnuti

    - Svetlo ma vlastni TransformationComposite stejne jako objekty, takze se muze hybat (obihat, letet s dronem, tocit se se svetem) bez specialniho kodu.
    - Validace v setterech - neplatna hodnota se opravi nebo odmitne hned, do shaderu se nikdy nedostane nesmysl.

## Na co si dat pozor

    - setPosition u svetla pripojeneho k transformaci je v lokalnich souradnicich (napr. u dronu je (0,-0.3,0) = kousek pod dronem).
    - setEnabled(false) svetlo jen preskoci pri plneni bufferu, neodstrani ho.

# LightBuffer

    LightBuffer je RAII obal nad Uniform Buffer Objectem (UBO), pres ktery se posilaji svetla do vsech shaderu najednou. Misto toho, aby se kazdemu shaderu nastavovaly desitky uniformu pro kazde svetlo, se jednou za snimek nahraje jeden blok pameti a vsechny shadery napojene na stejny binding point ho ctou.

## Popis implementace

    - constexpr int MAX_LIGHTS = 64 - maximalni pocet svetel, musi sedet se shadery.
    - constexpr GLuint LIGHT_BINDING_POINT = 0 - cislo "slotu", pres ktery se UBO propoji se shadery.
    - struct LightData - 5x glm::vec4 (position, color, attenuation, direction, params), presne zrcadli LightSource ve shaderu.
    - struct LightBufferHeader - glm::ivec4 info (x = pocet svetel) a glm::vec4 ambientLight. Odpovida zacatku bloku ve shaderu (lightInfo, ambientLight).
    - static_assert(sizeof(LightData) % 16 == 0) a static_assert(sizeof(LightBufferHeader) == 32) - kontrola uz pri prekladu, ze struktury splnuji zarovnani std140. Kdyby nekdo pridal treba vec3, preklad selze s jasnou hlaskou misto tichych chyb v osvetleni.
    - LightBuffer() - glGenBuffers, glBindBuffer(GL_UNIFORM_BUFFER) a glBufferData s velikosti hlavicka + 64 svetel a nullptr (jen alokace, data prijdou pozdeji). GL_DYNAMIC_DRAW, protoze data se meni casto. glBindBufferBase pripoji buffer na LIGHT_BINDING_POINT, pak se odbinduje.
    - ~LightBuffer() - glDeleteBuffers, pokud ubo existuje.
    - Kopirovani zakazano (= delete).
    - update(lights, ambientLight) const - pocet se orizne na MAX_LIGHTS, naplni se hlavicka a glBufferSubData nahraje hlavicku na offset 0 a svetla hned za ni (offset sizeof(header)). Svetla se nahravaji jen kdyz nejaka jsou, a jen tolik, kolik jich je (ne celych 64).

## Vztah k ostatnim tridam

    - Resources - vlastni jedinou instanci (unique_ptr) a pri vytvareni shaderu je napojuje na LIGHT_BINDING_POINT.
    - Scene - v draw() sestavi vector<LightData> ze zapnutych svetel a zavola update.
    - Light - toData() vytvari LightData.

## Navrhove rozhodnuti

    - Jeden UBO pro vsechny shadery - svetla se posilaji jednou za snimek, ne pro kazdy shader a objekt zvlast.
    - static_assert - chyba v zarovnani se odhali uz pri prekladu.

## Na co si dat pozor

    - LightBuffer vznika v konstruktoru Resources, takze Resources musi vzniknout az po inicializaci OpenGL (v Application::initialization je to dodrzeno).

# Camera

    Camera drzi pozici a smer pohledu, pocita view a projection matici a je to subjekt navrhoveho vzoru Observer. Pri kazde zmene oznami vsem registrovanym pozorovatelum (shaderum), ze se zmenila.

## Popis implementace

    - privatni promenne:
        - eye = (0,0,2) - pozice kamery
        - target = (0,0,-1) - SMER pohledu (jednotkovy vektor), ne bod, na ktery se kamera diva
        - up = (0,1,0)
        - alpha = 90, phi = -90 - sfericke uhly smeru pohledu. alpha je uhel od osy +Y (0 = nahoru, 90 = vodorovne, 180 = dolu), phi je otoceni kolem osy Y. Pro 90/-90 vychazi smer (0,0,-1).
        - fov = DEFAULT_FOV (60), aspect = 4/3, nearPlane = 0.1, farPlane = 250
        - speed = 1.5 jednotky za sekundu, sensitivity = 0.15 stupne na pixel mysi
        - std::vector<ICameraObserver*> observers - pozorovatele (nevlastnene ukazatele)
    - Camera() - zavola updateTarget(), aby target odpovidal uhlum.
    - attach(observer) - prida pozorovatele. notifyAll() - zavola onCameraChanged(this) u vsech.
    - getViewMatrix() - glm::lookAt(eye, eye + target, up). Protoze target je smer, bod pohledu je eye + target.
    - getProjectionMatrix() - glm::perspective(radians(fov), aspect, near, far).
    - setPosition(eye, alpha, phi) - nastavi vse najednou (alpha orizne na 1..179), prepocita target a oznami zmenu. Pouziva se pri mireni kamery (aimCamera, autopilot v ShowcaseInput).
    - updateTarget() - prevod sferickych souradnic na vektor: (sin a * cos p, cos a, sin a * sin p).
    - right() - vektor doprava = normalize(cross(target, up)), pro pohyb do stran.
    - rotate(dx, dy) - pohyb mysi zmeni phi a alpha (nasobeno sensitivity). alpha se orizne na 1..179, aby se kamera neprevratila pres pol (pri 0 nebo 180 by target byl rovnobezny s up a cross by vratil nulovy vektor).
    - moveForward/Backward/Left/Right/Up/Down(deltaTime) - posun eye ve smeru target / right / up o speed * deltaTime, pak notifyAll. Diky nasobeni deltaTime je rychlost nezavisla na FPS.
    - setFov - orizne na 1..179 a oznami. resetFov - vrati 60 (prostredni tlacitko mysi).
    - setAspect(width, height) - nulove rozmery ignoruje (minimalizovane okno), jinak prepocita pomer a oznami.
    - setSpeed - jen nastavi rychlost a neoznamuje (na matice nema vliv).
    - setFarPlane - zajisti, ze far je vzdy aspon o 0.01 dal nez near (jinak by projekce nefungovala), a oznami.
    - gettery getFov, getEye, getTarget, getFarPlane.

## Vztah k ostatnim tridam

    - ICameraObserver / ShaderProgram - pozorovatele.
    - Scene - vlastni kameru hodnotou, registruje do ni programy a pri aktivaci vola notifyAll.
    - SceneInput / ShowcaseInput - ovladaji kameru (WASD, mys, kolecko, autopilot).

## Navrhove rozhodnuti

    - Observer - kamera nevi nic o shaderech, jen o rozhrani.
    - Smer pohledu pres dva uhly - jednoduche ovladani mysi a snadne omezeni, aby se kamera neprevratila.

## Na co si dat pozor

    - Kazdy pohyb vola notifyAll, takze pri drzeni W i A se matice do shaderu posilaji dvakrat za snimek. Pri par shaderech to nevadi.
    - Chybi detach (viz ICameraObserver).

# Translate

    Translate je transformace posunu. Drzi vektor offset a vraci translacni matici. Na rozdil od CustomMatrix je menitelna, offset se da za behu zmenit, takze se pouziva i pro animace (houpani, padajici kostky, pohyb sveta).

## Popis implementace

    - class Translate : public Transformation
    - explicit Translate(const glm::vec3& offset) - explicit brani tiche konverzi z vec3.
    - getMatrix() const override - glm::translate(glm::mat4(1.0f), offset). Matice se pocita az pri volani, takze vzdy odpovida aktualnimu offsetu.
    - setOffset / getOffset - zmena a cteni posunu.
    - glm::vec3 offset - privatni.

## Vztah k ostatnim tridam

    - TransformationComposite - addStatic(Translate(...)) se okamzite zapece do matice, addDynamic(&translate) se cte kazdy snimek.
    - Scene - controlMove je Translate v base kompozitu (zdvih celeho sveta).
    - ShowcaseScene - houpani krychli v prstenci, sbiratelne kostky, padajici kostky, znacky pod drony.

# Rotate

    Rotate je transformace rotace o uhel (ve stupnich) kolem dane osy. Uhel jde menit za behu, osa je pevna.

## Popis implementace

    - Rotate(float angleDeg, const glm::vec3& axis) - uhel ve stupnich (citelnejsi nez radiany) a osa rotace.
    - getMatrix() - glm::rotate(mat4(1), glm::radians(angleDeg), axis). glm pracuje v radianech, proto prevod.
    - setAngle / getAngle - menitelny uhel.
    - privatni angleDeg a axis.

## Vztah k ostatnim tridam

    - Scene - controlSpin (otaceni celeho sveta kolem Y).
    - ShowcaseScene - natoceni objektu (addStatic) a "barrel roll" dronu pri otoceni (drone.roll, uhel se meni v updateDrones).

## Na co si dat pozor

    - Nulova osa by dala neplatnou matici (glm::rotate osu normalizuje a nulovy vektor normalizovat nejde).

# Scale

    Scale je transformace zmeny meritka. Ma dva konstruktory: pro rovnomerne meritko (jedno cislo) a pro ruzne meritko v osach (vec3).

## Popis implementace

    - explicit Scale(const glm::vec3& factor) - nerovnomerne meritko.
    - explicit Scale(float factor) - rovnomerne, uvnitr factor(factor, factor, factor).
    - getMatrix() - glm::scale(mat4(1), factor).
    - setFactor / getFactor.

## Vztah k ostatnim tridam

    - Scene - controlZoom (zmenseni sveta v rezimu miniatura).
    - ShowcaseScene - sloupce equalizeru (height se meni kazdy snimek), pulzovani slunce (sunPulse), zplosteni padajici kostky pri dopadu (squash).

## Na co si dat pozor

    - Nerovnomerne meritko deformuje normaly, proto DrawableObject posila do shaderu normalMatrix.
    - Meritko 0 by udelalo singularni matici (inverse v normalMatrix by nefungovalo), proto ShowcaseScene pouziva minimalne 0.001.

# TransformationComposite

    TransformationComposite sklada vic transformaci do jedne. Je to navrhovy vzor Composite: sam je Transformation a zaroven obsahuje seznam dalsich Transformation (i dalsich kompozitu). Vysledna matice je soucin matic vsech potomku v poradi, v jakem byly pridany.

## Popis implementace

    - class TransformationComposite : public Transformation
    - std::vector<Transformation*> children - poradi transformaci, nevlastnene ukazatele.
    - std::vector<std::unique_ptr<CustomMatrix>> ownedStatic - staticke matice, ktere kompozit vlastni.
    - addStatic(const Transformation& t) - transformace se predava referenci (muze byt docasny objekt, napr. addStatic(Translate(...))). Hned se z ni vezme matice. Kdyz je posledni potomek uz vlastni staticka matice, nova matice se do ni primo vynasobi (ownedStatic.back()->getMatrix() * matrix) a nova CustomMatrix nahradi starou. Jinak se vytvori nova CustomMatrix a prida se do children. Vysledek: libovolne mnozstvi po sobe jdoucich statickych transformaci je jen jedna matice.
    - addDynamic(Transformation* t) - prida ukazatel, ktery se bude cist pri kazdem getMatrix(). Kompozit ho nevlastni.
    - getMatrix() const override - zacne jednotkovou matici a postupne ji nasobi zprava maticemi potomku (matrix = matrix * child->getMatrix()). Prvni pridana transformace je tedy "nejvnejsi" (na vrchol se aplikuje jako posledni).
    - isOwnedStatic(t) - zjisti, jestli ukazatel patri do ownedStatic.
    - describe() - vrati retezec typu "D S D D S" (D = dynamicka, S = staticka), pouziva se pro vypis informaci o objektu (klavesa I).
    - size() - pocet kroku.

## Vztah k ostatnim tridam

    - CustomMatrix - uloziste zapecenych statickych matic.
    - Scene - base je kompozit (controlMove, controlSpin, controlZoom). makeComposite() vytvori novy kompozit, ktery jako prvni krok obsahuje base, takze kazdy objekt se hybe s celym svetem.
    - Light - kazde svetlo ma svuj kompozit.
    - ShowcaseScene - vsechny objekty jsou sestavene z kompozitu (napr. planeta: posun ke slunci, naklon, orbita, posun na polomer, nastup, rotace, meritko).

## Navrhove rozhodnuti

    - Composite - jednoduche i slozene transformace se pouzivaji stejne, kompozit muze obsahovat dalsi kompozit (base).
    - Rozliseni static / dynamic - staticke kroky se slouci do jedne matice uz pri stavbe sceny, za behu se nasobi mene matic. Dynamicke kroky (animace) se ctou kazdy snimek, takze se jejich zmena hned projevi.
    - Sdileni dynamickych transformaci - jedna DynamicRotate muze byt ve vice kompozitech (napr. planeta a jeji mesic sdileji orbitu, takze se pohybuji spolu).

## Na co si dat pozor

    - Dynamicke transformace kompozit nevlastni, musi zit aspon tak dlouho jako kompozit. V projektu je vlastni Scene (make<T>()).
    - Poradi pridavani je dulezite (posun a potom rotace neni totez co rotace a potom posun).
    - getMatrix se vola pro kazdy objekt kazdy snimek a base se pocita znovu pro kazdy objekt, vysledek se nikam neuklada.

# Oscillate

    Oscillate je casove zavisla rotace, ktera kmita tam a zpet podle sinusu (kyvadlo). Uhel = amplituda * sin(2 * pi * frekvence * cas + faze).

## Popis implementace

    - Oscillate(clock, amplitudeDeg, frequencyHz, axis, phaseDeg = 0) - odkaz na SceneClock, maximalni vychylka ve stupnich, frekvence v Hz (kmity za sekundu), osa a pocatecni faze.
    - startTime = clock.getTime() v konstruktoru - animace se pocita od okamziku vytvoreni.
    - getMatrix() - elapsed = cas sceny - startTime. Argument se pocita v double (presnost pri dlouhem behu), angle = amplituda * sin(argument), vysledkem je glm::rotate o tento uhel kolem osy.
    - setPhase / getPhase - zmena faze za behu (pouziva ShowcaseScene::cyclePendulums pro rezimy srovnana / spirala / nuzky).
    - const SceneClock& clock - reference, ne kopie, aby se cetl aktualni cas. Pri pauze hodin kyvadlo zamrzne, pri zrychleni kmita rychleji.

## Vztah k ostatnim tridam

    - SceneClock - zdroj casu.
    - ShowcaseScene - 15 kyvadel, kazde s jinou frekvenci (40 az 54 kmitu za minutu), vznika "pendulum wave".

## Na co si dat pozor

    - cyclePendulums pocita fazi z absolutniho casu getTime(), zatimco Oscillate pocita od startTime. Funguje to, protoze kyvadla vznikaji pri startu sceny, kdy je cas temer 0. Kdyby se kyvadlo vytvorilo pozdeji, faze by nesedely.

# LineMove

    LineMove je casove zavisla transformace, ktera pohybuje objektem tam a zpet po usecce z bodu a do bodu b konstantni rychlosti. Volitelne objekt natoci ve smeru pohybu.

## Popis implementace

    - LineMove(clock, a, b, unitsPerSecond, faceDirection = false).
    - struct State { float t; bool returning; } - vnitrni stav: t = poloha na usecce 0..1, returning = jestli se vraci z b do a.
    - getState() const - delka usecky. Kdyz je temer nulova (< 1e-4), vrati vychozi stav (deleni nulou). s = fmod(rychlost * uplynuly cas / delka, 2.0), jedna cesta tam a zpet trva 2 "delky". Kdyz s >= 1, objekt se vraci a t = 2 - s, jinak t = s. Vysledkem je pohyb typu ping-pong.
    - getT(), isReturning(), getPosition() - verejne cteni stavu (HUD telemetrie, znacka pod dronem, detekce otoceni).
    - getMatrix() - pozice = linearni interpolace (1 - t) * a + t * b, translacni matice. Kdyz je faceDirection, prida rotaci kolem Y o atan2(dx, dz) smeru pohybu, takze objekt miri dopredu (osa +Z modelu miri ve smeru jizdy). Podminka direction.x != 0 || direction.z != 0 - pri ciste svislem pohybu by atan2(0,0) nemelo smysl.
    - Stav se nikde neuklada, vzdy se pocita z casu. Transformace je bezstavova a je jedno, jak casto se vola.

## Vztah k ostatnim tridam

    - SceneClock - zdroj casu.
    - ShowcaseScene - drony nad scenou, jejich svetla (light->getTransformation().addDynamic(drone.path)) a znacky na zemi.

## Na co si dat pozor

    - Otoceni na konci usecky je okamzite (skok o 180 stupnu). ShowcaseScene to maskuje animaci "barrel roll" a zablesknutim.

# DynamicRotate

    DynamicRotate je rotace, ktera se otaci konstantni uhlovou rychlosti podle casu sceny. Uhel = pocatecni uhel + rychlost * uplynuly cas.

## Popis implementace

    - DynamicRotate(clock, degPerSecond, axis, startAngleDeg = 0).
    - privatni: clock (reference), degPerSecond, axis, startAngle, startTime (cas vytvoreni).
    - getAngle() - startAngle + degPerSecond * (clock.getTime() - startTime).
    - getMatrix() - glm::rotate o getAngle() kolem osy.
    - setSpeed(value) - pred zmenou rychlosti ulozi aktualni uhel jako novy startAngle a aktualni cas jako novy startTime. Diky tomu objekt pri zmene rychlosti neposkoci. Kdyby se zmenila jen rychlost, novy uhel by se pocital od puvodniho startu a objekt by se okamzite otocil jinam.
    - getSpeed().

## Vztah k ostatnim tridam

    - SceneClock - zdroj casu.
    - ShowcaseScene - otaceni slunce, obeh a rotace planet a mesicu, krychle v prstenci (setSpeed pri "boostu"), sroubovice DNA, vrtule dronu, obihajici svetla, padajici a sbiratelne kostky.

## Navrhove rozhodnuti

    - Uhel se pocita z casu, ne pricitanim delty kazdy snimek. Nehromadi se zaokrouhlovaci chyba a pauza i zrychleni hodin funguji automaticky.

# GrowScale

    GrowScale je casove zavisla zmena meritka z hodnoty from na hodnotu to za danou dobu, s plynulym rozjezdem a dojezdem (smoothstep), volitelne kolem bodu pivot. Pouziva se pro "nastup" objektu (vyrostou z nicoty), supernovu slunce a sebrani kostky.

## Popis implementace

    - GrowScale(clock, target, duration) - zkraceny konstruktor, deleguje na plny: roste z 0 na target ve vsech osach.
    - GrowScale(clock, from, to, duration, pivot = 0) - plny konstruktor, startTime = aktualni cas.
    - getProgress() - x = uplynuly cas / doba (pri doba <= 0 rovnou 1, deleni nulou), oriznuti na 0..1 a smoothstep x*x*(3 - 2x). Zacatek a konec animace jsou pomalejsi.
    - getCurrentScale() - glm::mix(from, to, progress).
    - getMatrix() - translate(pivot) * scale(aktualni) * translate(-pivot). Objekt se posune tak, aby pivot byl v pocatku, zmeni se meritko a vrati se zpet. Meritko se tak meni kolem pivotu, ne kolem pocatku.
    - restart() - startTime = aktualni cas, animace bezi znovu.
    - isFinished() - uplynuly cas >= doba.
    - settery a gettery pro from, to, duration, pivot - umoznuji animaci za behu predefinovat a spustit znovu (retezeni: zvetsit, pak zmensit).

## Vztah k ostatnim tridam

    - SceneClock - zdroj casu.
    - ShowcaseScene - intro() vytvari nastupove animace (vsechny se ukladaji do introAnimations a klavesa G je spusti znovu), sunBurst (supernova: 1 -> 2.4 -> 1), sbiratelne kostky (nastup, "pop" pri sebrani, zmizeni).

## Na co si dat pozor

    - Meritko from = 0 by udelalo singularni matici, proto intro zacina na 0.001.
    - setDuration hodnotu nekontroluje, zaporna doba se chova jako nulova (animace hned skonci).

# HudPlacement

    HudPlacement je transformace pro umisteni HUD prvku na obrazovku. Prvek se ukotvi k rohu nebo stredu obrazovky, posune o offset a zvetsi o scale. Pocita s pomerem stran okna, takze se pri zmene velikosti okna prvky nerozjedou.

## Popis implementace

    - enum class HudAnchor { Center, TopLeft, TopRight, BottomLeft, BottomRight } - kotva.
    - HudPlacement(const float* aspect, anchor, offset, scale) - aspect je UKAZATEL na pomer stran (Scene::hudAspect), ne hodnota. Kdyz se okno zmeni, Scene prepise hudAspect a vsechny HUD prvky to hned vidi bez dalsi aktualizace.
    - getMatrix() - podle kotvy urci roh v souradnicich ortho projekce: x je v rozsahu -aspect..aspect, y v rozsahu -1..1 (napr. TopLeft = (-aspect, 1)). Center = (0,0) (default ve switchi). Pozice = roh + offset, vysledkem je translate(pozice) * scale(scale).

## Vztah k ostatnim tridam

    - Scene - createHudObject() vytvori HudPlacement s &hudAspect a pri kresleni nastavi ortho projekci (-hudAspect, hudAspect, -1, 1).
    - ShowcaseScene - vsechny HUD prvky (vzorky svetel, prepinac prostredi, pocitadlo kostek, telemetrie dronu, rychlost casu, zamerny kriz).

## Na co si dat pozor

    - Offset se k rohu pricita primo, takze u pravych a hornich kotev musi byt zaporny, aby prvek byl uvnitr obrazovky (ShowcaseScene to tak dela, napr. TopRight s offsetem (-0.09, -0.09)).
    - Ukazatel aspect musi zit dele nez HudPlacement. Patri Scene, ktera vlastni i placement, takze je to splneno.

# CubeMap

    CubeMap je RAII obal nad OpenGL cube map texturou (6 ctvercovych obrazku = steny krychle). Pouziva se pro skybox. Funguje podobne jako Texture: nacte obrazky, nahraje je do GPU, pri chybe pouzije vychozi skybox a pri zaniku texturu smaze.

## Popis implementace

    - DEFAULT_DIRECTORY = "assets/textures/Skyboxes/skybox_default", DEFAULT_EXTENSION = ".png" - zalozni skybox.
    - CubeMap(directory, extension) - zkusi tryLoad(directory). Kdyz selze a nejde o vychozi skybox, vypise varovani a zkusi vychozi. Kdyz selze i ten, ukonci program.
    - facePaths(directory, extension) - static, sestavi 6 cest v poradi right, left, top, bottom, front, back. Poradi odpovida GL_TEXTURE_CUBE_MAP_POSITIVE_X, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z, ktere jdou v OpenGL po sobe, proto v cyklu staci POSITIVE_X + i.
    - tryLoad(directory, extension) - vraci bool misto ukonceni programu, aby slo zkusit zalozni skybox:
        - stbi_set_flip_vertically_on_load(false) - na rozdil od Texture se cube mapa neprevraci (cube mapy maji pocatek nahore)
        - nacte vsech 6 sten (vzdy RGBA) a kontroluje, jestli se soubor nacetl, jestli je ctvercovy a jestli maji vsechny steny stejnou velikost. OpenGL to u cube mapy vyzaduje.
        - kdyz je vse ok: glGenTextures, bind jako GL_TEXTURE_CUBE_MAP, glTexImage2D pro kazdou stenu, linearni filtrovani a GL_CLAMP_TO_EDGE ve vsech trech osach (S, T, R). Jinak by na hranach sten byly viditelne svy.
        - nakonec vzdy uvolni nactena data (i pri chybe, aby neunikala pamet)
    - ~CubeMap() - glDeleteTextures.
    - bind(unit) - glActiveTexture + glBindTexture(GL_TEXTURE_CUBE_MAP).
    - getFaceSize() - velikost steny v pixelech.
    - Kopirovani zakazano.

## Vztah k ostatnim tridam

    - Resources - getCubeMap() cachuje cube mapy podle slozky a pripony.
    - Skybox - pripoji cube mapu pri kresleni.

## Na co si dat pozor

    - stbi_set_flip_vertically_on_load je globalni nastaveni. Texture ho nastavuje na true a CubeMap na false. Funguje to, protoze kazda trida si ho nastavi pred vlastnim nacitanim.

# Skybox

    Skybox spojuje model krychle, shader a cube mapu do jednoho objektu, ktery umi nakreslit oblohu. Nic z toho nevlastni, jen drzi ukazatele na zdroje z Resources.

## Popis implementace

    - Skybox(Model* cube, ShaderProgram* program, CubeMap* cubeMap) - ulozi tri nevlastnene ukazatele.
    - draw() const:
        - glDepthMask(GL_FALSE) - vypne zapis do hloubkoveho bufferu. Skybox se kresli jako prvni a nic nezakryje, vsechno nakreslene potom ho prekryje.
        - program->use(), cubeMap->bind(0), setUniform("skybox", 0) - aktivace shaderu a cube mapy na jednotce 0.
        - cube->draw() - nakresli krychli.
        - glDepthMask(GL_TRUE) - zapis do hloubky zase zapne.
    - getProgram() - Scene ho pouzije pro registraci shaderu jako pozorovatele kamery.
    - Hlavicka pouziva jen dopredne deklarace, protoze trida drzi jen ukazatele.

## Vztah k ostatnim tridam

    - Resources - getSkybox() ho sestavi z modelu "skycube", shaderu skybox.vert/skybox.frag a cube mapy a vlastni ho.
    - Scene - setSkybox() nastavi aktivni skybox (nebo nullptr = bez oblohy) a v draw() ho kresli jako prvni.

## Na co si dat pozor

    - Scene pred kreslenim skyboxu nastavi stencil hodnotu 0, takze kliknuti na oblohu se bere jako kliknuti na pozadi.

# Model

    Model je RAII obal nad geometrii v GPU (VBO + VAO). Dostane pole floatu s vrcholy a popis jejich rozlozeni (VertexLayout), nahraje je do grafiky a umi je nakreslit. Nevi nic o pozici, barve ani shaderu, to resi DrawableObject.

## Popis implementace

    - namespace VertexAttribute { Position = 0, Normal = 1, Uv = 2, Color = 3 } - cisla lokaci atributu. Musi sedet s layout(location = ...) ve shaderech. Pojmenovane konstanty misto "magickych cisel".
    - struct VertexLayout - popis jednoho vrcholu:
        - floatsPerVertex - kolik floatu ma jeden vrchol (stride)
        - normalOffset, uvOffset, colorOffset - na kolikatem floatu atribut zacina, -1 = atribut chybi
        - staticke tovarni metody pro vsechny pouzivane kombinace: position (3), positionNormal (6), positionColor (6), positionNormalUv (8), positionNormalColor (9), positionNormalUvColor (11)
    - Model(const float* data, size_t floatCount, const VertexLayout& layout):
        - vertexCount = floatCount / floatsPerVertex, hasNormal/hasUV/hasColor podle offsetu >= 0
        - glGenBuffers + glBufferData - nahraje data do VBO (GL_STATIC_DRAW, geometrie se nemeni)
        - glGenVertexArrays + glBindVertexArray - VAO si pamatuje nastaveni atributu
        - enableAttribute(location, components, stride, offset) - pomocna static funkce: glEnableVertexAttribArray + glVertexAttribPointer. Offset se prevadi z floatu na bajty a predava jako void* (tak to OpenGL API vyzaduje).
        - pozice se zapne vzdy, ostatni atributy jen kdyz je layout obsahuje
        - nakonec se VAO i VBO odbinduji
    - ~Model() - smaze VAO i VBO.
    - Kopirovani zakazano.
    - draw() const - bindne VAO a pro chybejici atributy nastavi konstantni hodnotu (glVertexAttrib*): normala (0,1,0), UV (0,0), barva bila. Shader tak muze vzdy cist vsechny ctyri vstupy, i kdyz je model nema. Pak glDrawArrays(GL_TRIANGLES).

## Vztah k ostatnim tridam

    - BasicModels - zdroj dat pro zakladni modely.
    - ModelLoader - vytvari modely z nactenych souboru.
    - Resources - vlastni zakladni modely (getModel).
    - DrawableObject, Skybox - volaji draw().

## Navrhove rozhodnuti

    - Jeden obecny konstruktor + VertexLayout misto tridy pro kazdy format vrcholu.
    - Vychozi hodnoty chybejicich atributu - jeden shader funguje pro modely s barvou i bez ni, s texturou i bez ni.

## Na co si dat pozor

    - Model nekresli indexovane (glDrawElements), vrcholy se opakuji pro kazdy trojuhelnik. Pro jednoduche modely to staci.
    - Kdyz floatCount neni nasobek floatsPerVertex, posledni neuplny vrchol se zahodi (celociselne deleni).

# ModelLoader

    ModelLoader nacita 3D modely ze souboru (napr. .obj) pomoci knihovny Assimp a prevadi je na Model objekty. Model muze mit vic casti (meshu) s ruznymi materialy a texturami. Trida ma jen statickou metodu a zadny stav.

## Popis implementace

    - struct ModelPart - jedna cast modelu: unique_ptr<Model> (cast svuj model vlastni), cesta k texture, difuzni barva a Material.
    - struct LoadedModel - vector casti a bounding box (boundsMin, boundsMax).
    - static LoadedModel load(path, mergeColors = false):
        - Assimp::Importer::ReadFile s priznaky aiProcess_Triangulate (vse na trojuhelniky, Model kresli GL_TRIANGLES), aiProcess_GenSmoothNormals (dopocita normaly, kdyz chybi) a aiProcess_PreTransformVertices (zapece hierarchii uzlu primo do vrcholu, takze vysledek je jen seznam meshu bez transformaci)
        - kdyz se nacteni nepovede, vypise chybu Assimpu a ukonci program
        - directory = slozka souboru (cesty k texturam v .mtl jsou relativni k modelu)
        - bounds se inicializuji na +-1e30, aby prvni vrchol vzdy nastavil spravne hodnoty
        - layout: bez mergeColors positionNormalUv (8 floatu), s mergeColors positionNormalUvColor (11 floatu)
        - pro kazdy mesh: precte difuzni barvu materialu, projde vsechny trojuhelniky a pro kazdy vrchol vlozi pozici, normalu (nebo (0,1,0)) a UV (nebo 0). Model se tim "rozbali" z indexu na samostatne vrcholy. Pri mergeColors pripoji i barvu materialu. Prubezne aktualizuje bounding box.
        - bez mergeColors: kazdy mesh je samostatna ModelPart s vlastnim Model, barvou, materialem (readMaterial) a cestou k difuzni texture (zpetna lomitka nahrazena za / kvuli cestam z Windows)
        - s mergeColors: vsechny meshe se spoji do jednoho velkeho Model, barva je ve vrcholech. Mene objektu = mene vykreslovacich volani, ale prijde se o textury a materialy.
        - na konci vypise pocet casti a bounding box
    - readMaterial(aiMaterial*) - static funkce, precte zrcadlovou barvu a lesk. Lesk se pouzije jen kdyz je >= 1 (nektere exportery davaji 0).

## Vztah k ostatnim tridam

    - Model - vytvari ho pro kazdou cast.
    - Material - kazda cast ma svuj material.
    - Resources - getObjModel() nacteny model cachuje (klic = cesta, pripadne "|merged").
    - Scene - createObjModel() z casti vytvori DrawableObjecty a nastavi texturu nebo barvu.

## Na co si dat pozor

    - Face s jinym poctem vrcholu nez 3 se preskoci (po Triangulate tam muzou zustat jen cary a body).
    - Ambientni a difuzni slozka materialu se z Assimpu necte, zustava vychozi hodnota Material.

# DrawableObject

    DrawableObject je jeden vykreslitelny objekt sceny. Spojuje model (geometrii), shader, transformaci, barvu a material. Krome materialu a barvy nic z toho nevlastni: model a shader jsou sdilene z Resources, transformaci vlastni Scene. Navic nese informace pro vyber mysi (id) a druh objektu (Kind).

## Popis implementace

    - enum class Kind { Normal, Ground, Collectible, Moving } - druh objektu. Podle nej se rozhoduje, co se stane po kliknuti (napr. Ground = shodit kostku).
    - DrawableObject(Model*, ShaderProgram*, Transformation*) - ulozi tri nevlastnene ukazatele.
    - privatni: model, program, transformation, color = bila, material (vlastni kopie), visible = true, id = 0, kind = Normal.
    - draw() const:
        - neviditelny objekt se preskoci
        - program->use() a nahrani modelMatrix z transformation->getMatrix()
        - normalMatrix = transpose(inverse(mat3(model))) se pocita jen kdyz ho shader ma (HUD shader ne). Spravne transformuje normaly i pri nerovnomernem skalovani.
        - objectColor a material.apply(program)
        - glStencilFunc(GL_ALWAYS, id, 0xFF) - do stencil bufferu se na misto objektu zapise jeho id (spolu s glStencilOp(KEEP, KEEP, REPLACE) z Application). Pozdeji se podle pixelu pod mysi zjisti, na co se kliklo.
        - model->draw()
    - kindName(kind) - static, textovy nazev druhu pro vypis.
    - settery a gettery pro vsechny vlastnosti. setTexture/getTexture pracuji primo s material.texture.

## Vztah k ostatnim tridam

    - Model, ShaderProgram, Transformation, Texture - nevlastnene zavislosti.
    - Material - vlastni kopii.
    - Scene - vlastni vsechny objekty (unique_ptr), prideluje id, kresli je a hleda podle id.

## Navrhove rozhodnuti

    - Sdileni zdroju - stovky objektu muzou pouzivat jeden model a jeden shader.
    - Vyber objektu pres stencil buffer - presny po pixelech, nezavisly na tvaru objektu, bez pocitani paprsku. Cena: maximalne 255 vybiratelnych objektu (8bitovy stencil).

## Na co si dat pozor

    - inverse se pocita pro kazdy objekt kazdy snimek. Pri singularni matici (meritko 0) by vysly neplatne normaly.
    - Objekt s id 0 neni vybiratelny (0 = pozadi).

# Resources

    Resources je centralni sprava zdroju (cache, princip Flyweight). Vlastni vsechny shadery, modely, textury, cube mapy, skyboxy a buffer svetel. Kdo zdroj potrebuje, zepta se pres get...() a dostane ukazatel. Kdyz zdroj jeste neexistuje, vytvori se, jinak se vrati existujici. Kazdy zdroj je tak v pameti (a v GPU) jen jednou a sdili ho vsechny sceny.

## Popis implementace

    - privatni mapy std::unordered_map<std::string, std::unique_ptr<T>> pro kazdy typ zdroje. Klic je retezec, hodnota vlastnici ukazatel. lightBuffer je jediny (unique_ptr).
    - Resources() - vytvori LightBuffer. ~Resources() = default - unique_ptr vse uvolni samy. Destruktor je definovany v .cpp, protoze v hlavicce jsou nektere typy jen dopredne deklarovane (unique_ptr potrebuje znat cely typ v miste destrukce).
    - Kopirovani zakazano.
    - normalize(path) - std::filesystem::path(path).lexically_normal().generic_string(). Sjednoti zapis cesty ("a/./b", "a//b", zpetna lomitka), aby stejny soubor nemel dva klice v cache.
    - getShader(vertexFile, fragmentFile) - klic "vertex|fragment", soubory se hledaji ve slozce assets/shaders/. Novy program napoji na uniform blok LightBuffer, pokud ho shader ma.
    - getModel(name) - zakladni modely podle jmena: "skycube" (jen pozice), "floor" (pozice, normala, UV), "cube" (pozice, normala, UV), "hudQuad" (pozice, barva). Data jsou z BasicModels.h, pocet floatu se pocita sizeof(pole) / sizeof(float). Nezname jmeno vrati nullptr s varovanim.
    - getObjModel(path, mergeColors) - nacte model pres ModelLoader, klic je cesta, pri mergeColors s priponou "|merged". Vraci const referenci na LoadedModel.
    - getTexture(path, options) - klic = cesta + "|clamp" / "|nomip" podle voleb.
    - getCubeMap(directory, extension) - klic "slozka|pripona".
    - getSkybox(directory, extension) - sestavi Skybox z modelu "skycube", shaderu skybox.vert/skybox.frag a cube mapy. Vsechno opet pres Resources, takze se nic nevytvari dvakrat.
    - getLightBuffer().

## Vztah k ostatnim tridam

    - vlastni ShaderProgram, Model, LoadedModel, Texture, CubeMap, Skybox, LightBuffer.
    - Application - vlastni Resources (unique_ptr) a predava ho scenam referenci.
    - Scene a potomci - ziskavaji zdroje pres resources.get...().

## Navrhove rozhodnuti

    - Cache s lazy inicializaci - zdroj vznikne az pri prvnim pouziti.
    - Vlastnictvi na jednom miste - ostatni tridy drzi jen nevlastnene ukazatele, nikdo jiny zdroje nemaze.
    - Normalizace klicu - ruzne zapsana stejna cesta = jeden zdroj.

## Na co si dat pozor

    - Resources se musi znicit pred glfwTerminate (destruktory volaji OpenGL) a az po scenach (sceny drzi ukazatele na zdroje). Application::run to tak dela.
    - Zdroje se neuvolnuji, dokud Resources zije, i kdyz je uz nikdo nepouziva.
    - Z BasicModels se pouzivaji jen skycube, texturedFloor, cube a hudQuad. triangle, square, ground a centeredTriangle zustaly z drivejsich cviceni.

# Scene

    Scene je zakladni trida vsech scen. Vlastni objekty, transformace, svetla, kameru a hodiny a stara se o vykresleni sceny, HUD a vyber objektu mysi. Konkretni sceny (ShowcaseScene, TestScene) z ni dedi a prepisuji create() a update(), coz je navrhovy vzor Template Method.

## Popis implementace

    - struct PickResult - vysledek kliknuti: objekt (nebo nullptr), jestli je HUD, jestli mame bod ve svete a jeho pozice.
    - explicit Scene(Resources&) - ulozi referenci na Resources, sestavi base kompozit (controlMove, controlSpin, controlZoom) a ziska LightBuffer.
    - virtual ~Scene() - virtualni, protoze sceny se drzi pres unique_ptr<Scene>.
    - Kopirovani zakazano.
    - build() - createDefaults() a potom virtualni create(). Neni to v konstruktoru, protoze v konstruktoru predka se virtualni metody potomka nevolaji (objekt potomka jeste neexistuje). Application proto vola build() az po vytvoreni sceny.
    - createDefaults() - vychozi obsah kazde sceny: pozice kamery (0, 1.7, 0) = vyska oci, podlaha (model "floor", shader lit + phong, vychozi textura, Kind::Ground), vychozi skybox a HUD shader.
    - create() a update(deltaTime) - virtualni, v zakladni scene prazdne. Potomci je prepisuji.
    - tick(deltaTime) - posune hodiny (clock.advance) a zavola update. Poradi zajisti, ze update uz vidi novy cas.
    - getClock().
    - activate(width, height) - vola ho Application pri prepnuti sceny: resize a camera.notifyAll() (sdilene shadery dostanou matice teto kamery).
    - resize(width, height) - nulove rozmery ignoruje. Ulozi velikost viewportu (pro pick), prepocita hudAspect a pomer stran kamery.
    - createObject(model, program, transformation) - vytvori DrawableObject (vlastni ho ownedObjects), zaregistruje program jako pozorovatele kamery, prideli id a prida objekt do objects.
    - createStaticObject(...) - stejne, ale bez id (neni vybiratelny) a do staticObjects. Pro dekorace (hvezdy, tecky obeznych drah, tyce kyvadel...), aby nespotrebovavaly omezenych 255 id.
    - createObjModel(path, program, transformation, mergeColors) - nacte model pres Resources a pro kazdou cast vytvori objekt. Kdyz cast ma texturu a soubor existuje, nastavi ji, jinak pouzije difuzni barvu (a pripadne varuje).
    - makeComposite() - novy TransformationComposite (vlastni ho Scene), jehoz prvni krok je base. Vsechny objekty vytvorene timto zpusobem se hybou s celym svetem.
    - make<T>(args...) - sablonova tovarni metoda: vytvori libovolnou transformaci, Scene ji vlastni (transformations) a vrati ukazatel. std::forward predava argumenty beze zmeny (perfect forwarding).
    - registerProgram(program) - kdyz program jeste neni registrovany, prida ho, pripoji ke kamere a hned mu posle aktualni matice (onCameraChanged).
    - allocateId() - nejdriv recykluje uvolnena id (freeIds), jinak bere nove (1..255). Pri vycerpani varuje a vrati 0 (objekt nebude vybiratelny). 255 je limit 8bitoveho stencil bufferu, 0 je pozadi.
    - releaseId(object) - vrati id do freeIds a objektu nastavi 0.
    - remove(object) - najde objekt v objects, hudObjects nebo staticObjects, odebere ho, uvolni id, pripadne vynuluje ground a smaze ho z ownedObjects (tim se zavola destruktor). Vraci true/false.
    - getObjectById(id) - prohleda objects a hudObjects (staticke objekty id nemaji).
    - createHudObject(model, anchor, offset, scale) - vytvori HudPlacement s ukazatelem na hudAspect, objekt s HUD shaderem, prideli id (HUD je klikatelny) a prida ho do hudObjects. HUD program se nebere jako pozorovatel kamery.
    - isHud(object).
    - pick(x, y) const - vyber objektu pod mysi:
        - mimo viewport vrati prazdny vysledek
        - glY = vyska - 1 - y, protoze OpenGL ma pocatek vlevo dole a okno vlevo nahore
        - glReadPixels precte hloubku a stencil hodnotu (= id objektu) pixelu
        - objekt se najde podle id a urci se, jestli je HUD
        - kdyz to neni HUD a hloubka < 1 (neco tam je), glm::unProject prevede (x, y, hloubka) zpet na bod ve svete. Pouziva se pro shozeni kostky na misto kliknuti.
    - draw() const - vykresleni snimku:
        - skybox (se stencil 0, takze je to pozadi)
        - sestaveni vector<LightData> ze zapnutych svetel (max MAX_LIGHTS, pri prekroceni jednou varuje) a LightBuffer::update
        - staticke objekty, potom normalni objekty
        - HUD: aktivace HUD shaderu, nastaveni ortho projekce (-hudAspect..hudAspect, -1..1), vypnuti hloubkoveho testu (HUD je vzdy nahore a prepise i stencil id), kresleni, zapnuti hloubkoveho testu
        - ShaderProgram::unbind()
    - getGround, removeGround, getCamera.
    - addLight(attachToBase = true) - vytvori svetlo (vlastni ho lights). Kdyz attachToBase, pripoji do jeho transformace base, takze se svetlo hybe se svetem. Baterka (kopiruje kameru) se vytvari s false.
    - removeLight(light), getLights.
    - setAmbientLight / getAmbientLight, setClearColor / getClearColor (barvu pouziva Application pri glClear).
    - setSkybox(skybox) - nastavi skybox a zaregistruje jeho program jako pozorovatele kamery. nullptr = bez skyboxu.
    - getBase, getMove, getSpin, getZoom - pristup k ovladani celeho sveta.
    - protected: Resources& resources - potomci ho pouzivaji primo.

## Vztah k ostatnim tridam

    - vlastni: Camera, SceneClock, DrawableObject (ownedObjects), Transformation (transformations), Light, Translate/Rotate/Scale/TransformationComposite (base).
    - nevlastni: Resources, ShaderProgram, Skybox, LightBuffer.
    - Application - vlastni sceny, vola build, tick, draw, activate, resize.
    - SceneInput - ovlada scenu (kamera, pick).

## Navrhove rozhodnuti

    - Template Method - build() a tick() urcuji poradi kroku, potomek doplni jen create() a update().
    - Tovarni metody (createObject, makeComposite, make<T>) - scena objekty vytvori a hned vlastni, potomek se nemusi starat o pamet.
    - Scene graph pres base - jeden kompozit nad celym svetem umoznuje svet tocit, zvedat a zmensovat najednou (rezim miniatura).
    - Vyber pres stencil buffer.

## Na co si dat pozor

    - remove() smaze objekt, ale jeho transformace v transformations zustanou az do konce sceny.
    - Ukazatele na smazany objekt, ktere si drzi potomek (napr. ShowcaseScene), se musi vycistit rucne. Proto ShowcaseScene vola forget().
    - static bool warned v draw() je spolecny pro vsechny sceny (varovani o prilis mnoha svetlech se vypise jen jednou za beh programu).

# SceneInput

    SceneInput je zakladni ovladani sceny. Prijima udalosti z klavesnice a mysi (preposila je Application), ovlada kameru a zpracovava kliknuti mysi na objekty. Konkretni sceny maji vlastni potomky (ShowcaseInput, TestSceneInput), kteri prepisuji virtualni metody.

## Popis implementace

    - konstanty v .cpp: FOV_STEP = 15 (krok zoomu koleckem), FOV_MIN = 15, FOV_MAX = 120, SPRINT_MULTIPLIER = 3 (Shift zrychli pohyb).
    - explicit SceneInput(Scene&) - drzi referenci na ovladanou scenu (protected, potomci ji pouzivaji).
    - virtual ~SceneInput() = default - drzi se pres unique_ptr<SceneInput>.
    - onKey(...) - v zakladu nic nedela. (void)parametr potlacuje varovani o nepouzitych parametrech.
    - onMouseButton(...) - leve tlacitko: ulozi pozici kurzoru a nastavi clickPending (kliknuti se nezpracuje hned, viz processPendingClick). Prostredni tlacitko: resetFov.
    - onMouseMove(window, dx, dy) - pri drzenem pravem tlacitku otaci kamerou.
    - onScroll(window, dx, dy) - kolecko meni FOV (zoom), oriznuto na 15..120.
    - update(window, deltaTime) - vola se kazdy snimek. Lambda pressed(key) zjisti stav klavesy (glfwGetKey). WASD pohyb, E nahoru, Q dolu, Shift sprint. Pouziva se dotazovani na stav (polling), ne udalosti, protoze pohyb ma byt plynuly po celou dobu drzeni klavesy.
    - processPendingClick(window) - vola ho Application po draw(), kdy je ve framebufferu aktualni snimek a stencil s id. Prepocita souradnice okna na souradnice framebufferu (na HiDPI/Retina displejich se lisi), zavola scene.pick() a podle vysledku zavola jednu z metod onBackgroundClick, onHudClick, onGroundClick (Kind::Ground) nebo onObjectClick.
    - onHudClick, onGroundClick, onObjectClick, onBackgroundClick - protected virtualni "hacky", v zakladu prazdne. Potomek si prepise jen ty, ktere potrebuje.
    - privatni: clickPending, clickX, clickY.

## Vztah k ostatnim tridam

    - Scene - ovlada jeji kameru a vola pick.
    - Application - preposila udalosti a kazdy snimek vola update a processPendingClick.
    - ShowcaseInput, TestSceneInput - potomci.

## Navrhove rozhodnuti

    - Odlozene zpracovani kliknuti - glReadPixels cte framebuffer, takze kliknuti se musi zpracovat az po vykresleni, ne v callbacku GLFW.
    - Template Method / hacky - processPendingClick rozhodne o typu kliknuti, potomek jen reaguje.
    - Ovladani oddelene od sceny - scena nevi nic o GLFW a klavesach.

# MainInput

    MainInput zpracovava globalni klavesy, ktere funguji ve vsech scenach (ukonceni, prepnuti sceny, fullscreen). Klavesy se nejdriv posilaji jemu a teprve kdyz je nezpracuje, jdou do vstupu aktivni sceny.

## Popis implementace

    - explicit MainInput(Application& app) - drzi referenci na aplikaci.
    - bool onKey(key, action, mods) - reaguje jen na stisk (GLFW_PRESS). Vraci true, kdyz klavesu zpracoval (dal se uz neposila):
        - ESC - app.close()
        - TAB - dalsi scena, Shift + TAB predchozi (mods & GLFW_MOD_SHIFT)
        - F11 - prepnuti fullscreen
        - jine klavesy vraci false

## Vztah k ostatnim tridam

    - Application - vlastni ho jako clena a vola ho v keyCallback. MainInput vola verejne metody close, switchScene a toggleFullscreen.

## Navrhove rozhodnuti

    - Zjednoduseny Chain of Responsibility - udalost jde nejdriv globalnimu ovladaci, pak scene. Navratova hodnota bool rika, jestli se ma pokracovat.

# ShowcaseInput

    ShowcaseInput je ovladani hlavni demo sceny ShowcaseScene. Rozsiruje SceneInput o desitky klaves (prepinani efektu, kamery, casu), interakci mysi s objekty a "autopilota" kamery (prelet k bodum, oblet sceny, dolly zoom).

## Popis implementace

    - konstanty: FLIGHT_DURATION 1.8 s, VERTIGO_DURATION 5 s, VERTIGO_CLOSEST 0.4 (nejblizsi priblizeni = 40 % vzdalenosti), parametry obletu (rychlost, polomer, vyska, bod pohledu), rozsah rychlosti casu 0.125..4, rychlost otaceni a zdvihu sveta.
    - enum class Autopilot { None, Flight, Orbit, Vertigo } - aktualni rezim automaticke kamery (stavovy automat).
    - struct Shot - pojmenovany pohled (nazev, pozice oka, bod pohledu).
    - printHelp() - static, vypise napovedu do konzole (pri prvnim update a na F1).
    - onKey - nejdriv zavola SceneInput::onKey, pak reaguje na stisk:
        - 1-6 - flyTo na predpripraveny pohled (pole SHOTS, index key - GLFW_KEY_1)
        - C oblet, V dolly zoom, N prostredi, M miniatura, R reset sveta, P rezim kyvadel, K sbirani kostek, O dalsi objekt, I info o objektu, Space pauza, +/- rychlost casu, F baterka, L chase svetla, G znovu nastup sceny, H HUD, Backspace uklid kostek, F1 napoveda
        - vetsina akci vola metodu ShowcaseScene a vypise stav do konzole
    - onMouseMove - kdyz uzivatel otaci kamerou mysi, vypne autopilota (uzivatel ma prednost), pak zavola predka.
    - update(window, deltaTime):
        - WASDQE vypne autopilota
        - sipky: otaceni (vlevo/vpravo) a zdvih (nahoru/dolu) sveta pres nudgeWorld, nasobeno deltaTime
        - SceneInput::update (pohyb kamery)
        - Flight: postup 0..1, smoothstep, linearni interpolace pozice i bodu pohledu a navic oblouk nahoru (sin(t * pi), vetsi pri delsim letu). Na konci se autopilot vypne.
        - Orbit: polomer a vyska se plynule priblizi k cilovym (exponencialni blend nezavisly na FPS: 1 - exp(-k * dt)), uhel roste, kamera se lehce houpe nahoru a dolu a miri na stred sceny
        - Vertigo (dolly zoom, efekt z filmu Vertigo): kamera se priblizuje ke slunci a zaroven rozsiruje FOV tak, aby sirka zaberu v miste slunce zustala stejna: fov = 2 * atan(sirka / (2 * vzdalenost)). Slunce zustava stejne velke, okoli se "natahuje". Na konci se FOV vrati.
    - onHudClick, onGroundClick, onObjectClick - prepsane hacky, preposilaji kliknuti do ShowcaseScene (hudClick, spawnCube, interact).
    - flyTo(shot) - ulozi pocatecni a cilovou pozici. Pocatecni bod pohledu se dopocita ze soucasneho smeru kamery ve stejne vzdalenosti, jakou ma cilovy zaber, aby se kamera behem letu plynule stocila.
    - startOrbit() - vychozi uhel, polomer a vyska se vezmou ze soucasne pozice kamery (atan2), takze oblet zacne plynule tam, kde kamera je.
    - startVertigo() - cil = svetova pozice slunce (toWorld, protoze svet muze byt otoceny nebo zmenseny). Z aktualni vzdalenosti a FOV spocita sirku zaberu. Kdyz je kamera prilis blizko (< 1), efekt nespusti.
    - stopAutopilot() - pri preruseni dolly zoomu vrati puvodni FOV.
    - aim(eye, lookAt) - prevod "kamera v bode eye se diva na lookAt" na uhly kamery: alpha = acos(dir.y), phi = atan2(dir.z, dir.x). Je to inverze Camera::updateTarget.
    - changeSpeed(factor) - nasobi rychlost casu 2x nebo 0.5x, oriznuto na 0.125..4.
    - togglePause().

## Vztah k ostatnim tridam

    - SceneInput - predek.
    - ShowcaseScene - ovladana scena (typovana reference showcase vedle zdedene Scene& scene).
    - Camera, SceneClock - primo ovladane.

## Navrhove rozhodnuti

    - Stavovy automat autopilota - vzdy bezi nejvyse jeden rezim, kazdy novy rezim nejdriv zastavi predchozi.
    - Rucni ovladani ma prednost pred automatikou.
    - Exponencialni blend a smoothstep - plynuly pohyb nezavisly na FPS.

## Na co si dat pozor

    - Diky orezani rychlosti casu na 0.125..4 nemuze rychlost klesnout na 0, takze log2 v ShowcaseScene::updateHud je vzdy v bezpecnem rozsahu.
    - WASDQE pouziva i predek pro pohyb kamery, ShowcaseInput je navic jen pouzije pro vypnuti autopilota.

# TestSceneInput

    TestSceneInput je minimalni ovladani pro TestScene. Slouzi jako sablona a priklad, jak napojit vlastni vstup na novou scenu.

## Popis implementace

    - class TestSceneInput : public SceneInput.
    - explicit TestSceneInput(TestScene& scene) - preda scenu predkovi (SceneInput(scene)) a navic si ulozi typovanou referenci testScene, pres kterou by mohl volat metody specificke pro TestScene.
    - onKey(...) override - zatim jen zavola SceneInput::onKey. Pripravene misto pro klavesy testovaci sceny.
    - Pohyb kamery, zoom a kliknuti dedi ze SceneInput beze zmeny.

## Na co si dat pozor

    - testScene se zatim nepouziva (prekladac muze varovat o nepouzitem clenu).

# ShowcaseScene

    ShowcaseScene je hlavni demonstracni scena projektu. Ukazuje vsechny casti enginu najednou: hierarchicke transformace (slunecni soustava), materialy (prstenec 10 materialu), casove animace (kyvadla, sroubovice, drony), ruzne typy svetel (bodova, smerove, reflektor, baterka), skybox a prostredi (noc, vesmir, mlha), HUD, vyber objektu mysi a malou hru se sbiranim kostek. Vsechny objekty jsou slozene jen z jednoho modelu krychle a ruznych transformaci.

## Popis implementace

### Pomocne veci v ShowcaseScene.cpp

    - class ShearWobble : public Transformation - lokalni transformace (jen v tomto souboru). Matice zkoseni (shear), kde amount urcuje naklon. Ma i velocity, protoze se nad ni v update pocita tlumena pruzina: brana se po kliknuti rozhoupe a postupne zastavi.
    - hexColor(0xRRGGBB) - prevod barvy z hex zapisu na vec3 0..1 (bitove posuny a maska 0xFF).
    - hue(h, saturation, value) - prevod HSV na RGB (kompaktni vzorec bez vetveni). Pouziva se pro duhove barvy.
    - glowMaterial() - "svitici" material: ambient = 1 / AMBIENT, diffuse a specular 0. Ve shaderu je ambientni slozka material.ambient * ambientLight * barva = (1 / 0.12) * 0.12 * barva = barva. Objekt ma tedy vzdy plnou barvu bez ohledu na svetla a vypada jako zdroj svetla.
    - blackMaterial(), stoneMaterial(), glossyMaterial() - dalsi vlastni materialy.
    - materialPalette() - vsech 10 predpripravenych materialu z Material.
    - aimCamera(camera, eye, target) - namiri kameru (stejny prevod jako ShowcaseInput::aim).
    - smooth01(x) - smoothstep.
    - konstanty pozic casti sceny (SUN_POSITION, RING_RADIUS, HELIX_CENTER, EQUALIZER_CENTER, GATE_POSITION, ...), limity (MAX_SPAWNED 16, COLLECTIBLE_COUNT 8), fyzika (GRAVITY 18, BOUNCE 0.55) a barvy prostredi.

### Struktury v hlavicce

    - Column (sloupec equalizeru), Bobber (houpajici se posun), Spawned (shozena kostka s fyzikou), Flash (zablesknuti objektu), Drone, Pendulum, Collectible - skupiny ukazatelu a stavu, ktere patri k jednomu animovanemu prvku. Objekty a transformace vlastni Scene, struktury drzi jen ukazatele pro rizeni animace.
    - Environment { Night, Space, Fog }.
    - std::mt19937 random{ 20251117u } - generator nahodnych cisel s pevnym seedem, takze hvezdy jsou pri kazdem spusteni na stejnem miste.

### Pomocne metody

    - intro(duration, from, pivot) - vytvori GrowScale z from na 1 a ulozi ho do introAnimations (G je spusti znovu).
    - spawn(transformation, material, color, textured, decorative) - vytvori objekt z krychle a lit shaderu. decorative = createStaticObject (bez id, neda se vybrat), jinak createObject.
    - hudSquare(anchor, offset, scale) - HUD ctverec z krychle.
    - toWorld(local) - prevede bod ze souradnic sveta pred base transformaci do skutecnych svetovych souradnic (svet muze byt otoceny, zvednuty nebo zmenseny).

### create()

    - ziska model krychle, shader lit.vert + showcase.frag, vychozi texturu a jeji "ostrou" variantu (bez opakovani a mipmap)
    - ambientni svetlo 0.12, kamera na prehledovy pohled, rychlost kamery 3
    - podlaze (z createDefaults) zmeni barvu a shader na showcase shader
    - postavi vsechny casti (build...) a nastavi prostredi

### buildSolarSystem()

    - podstavec (kamen s texturou) a zlata deska
    - slunce = dve otocene krychle se svitecim materialem. Kazda ma retezec: posun ke slunci, nastup (sdileny), sunBurst (supernova), sunPulse (tep), vlastni DynamicRotate (opacne smery) a meritko.
    - bodove svetlo slunce
    - 3 planety (lokalni struct Planet: polomer, velikost, rychlost, naklon drahy, start, material, mesic). Transformace planety: posun ke slunci -> naklon drahy (Rotate kolem Z) -> orbita (DynamicRotate kolem Y) -> posun na polomer -> nastup -> vlastni rotace -> meritko. To je ukazka hierarchie: planeta se toci kolem sebe a zaroven obiha.
    - mesic sdili prvni cast retezce s planetou (stejne objekty orbit a grow) a za ni prida vlastni obeh kolem planety. Proto se mesic pohybuje s planetou.
    - 40 malych tecek na kazde draze (decorative).

### buildMaterialRing()

    - 10 podstavcu do kruhu. Na kazdem je krychle s jinym materialem z palety, ktera se toci (DynamicRotate), houpe (Bobber) a nastupuje (intro s postupnym zpozdenim).
    - pod kazdou krychli svitici lem a nad ni barevne bodove svetlo. Barvy jsou rovnomerne po duze (hue(i / count)).

### buildPendulumWave()

    - dva sloupky a chromovy tram.
    - 15 kyvadel. Frekvence kyvadla i je (40 + i) / 60 Hz. Delka tyce se dopocita z fyzikalniho vzorce kyvadla omega = sqrt(g / L), tedy L = g / omega^2 (vynasobeno lengthScale, aby se kyvadla vesla do sceny). Diky ruznym frekvencim vytvari kyvadla vlny ("pendulum wave") a po 60 s se zase srovnaji.
    - tyc a zavazi sdileji stejny Oscillate (osa X).
    - jedno svetlo.

### buildHelix()

    - "DNA" sroubovice: 18 pater, v kazdem dve krychle (rubin a smaragd) na protilehlych stranach a pricka mezi nimi. Kazde patro je pootocene o 34 stupnu a o 0.32 vys. Cela sroubovice sdili jeden GrowScale (vyroste ze zeme, meritko Y z 0.001) a jednu DynamicRotate.

### buildEqualizer()

    - mrizka 7x7 sloupcu. Kazdy ma dynamicke Scale (height), ktere se meni v update. Vyska se meni od spodni hrany, protoze za Scale nasleduje posun o 0.5 nahoru (sloupec "roste ze zeme").

### buildDrones()

    - 2 drony. Kazdy leti po LineMove (s natacenim ve smeru letu) a ma Rotate roll pro barrel roll.
    - lambda part(offset, size) vytvori kompozit casti dronu, aby se nemusel opakovat stejny zacatek retezce.
    - telo, kabina, svitici spodek a 4 vrtule (DynamicRotate 1440 stupnu/s, smer podle rohu, aby se sousedni vrtule tocily opacne).
    - znacka na zemi pod dronem (Translate, pozice se kopiruje v updateDrones).
    - svetlo dronu - jeho transformace obsahuje LineMove dronu, takze leti s nim.

### buildLights()

    - mesic - smerove svetlo (Directional).
    - reflektor (Spot) vysoko nad scenou, v update miri na krychle prstence.
    - baterka (Spot) - addLight(false), neni pripojena ke svetu, protoze kopiruje kameru. Na zacatku je vypnuta.
    - 3 obihajici barevna svetla se svitici znackou. Svetlo i znacka sdileji stejnou DynamicRotate (orbit).

### buildGate()

    - brana ze dvou naklonenych sloupu a horni desky. Naklon je CustomMatrix se zkosenim (shear), druhy sloup je zrcadleny pomoci CustomMatrix se scale(-1, 1, 1). Ukazka CustomMatrix pro transformace, ktere nejsou posun, rotace ani meritko.
    - vsechny casti maji gateWobble (ShearWobble), takze se po kliknuti brana rozhoupe.
    - neonovy pas pod horni deskou (barva se meni v update).

### buildSky()

    - dome - obri cerna krychle (meritko 260), viditelna jen v mlze.
    - 380 hvezd v nahodnych smerech (normalni rozdeleni ve 3D + normalizace = rovnomerne po kouli) ve vzdalenosti 420..600. Pouziva make<TransformationComposite>() misto makeComposite(), takze hvezdy nejsou pripojene k base a nehybou se, kdyz se otaci svet. Hvezdy jsou za far plane 250, viditelne jsou jen ve vesmiru (far plane 900).

### buildHud()

    - vlevo dole barevne vzorky svetel prstence (klik = zapnout/vypnout svetlo), vlevo nahore 3 ctverce prostredi (klik = prepnuti), vpravo nahore pocitadlo sebranych kostek, vpravo dole 2 rady telemetrie dronu a indikator rychlosti casu, uprostred zamerny kriz baterky.

### Prostredi

    - applyEnvironment() - podle prostredi nastavi skybox (jen v noci), barvu pozadi, mlhu, rozptyl, rim light, far plane kamery a intenzitu mesice. Ve vesmiru schova podlahu, v mlze ukaze dome. Uniformy mlhy a rimu se nastavi primo do showcase shaderu.
    - cycleEnvironment() - dalsi prostredi (modulo 3), vraci nazev pro vypis.

### Pohyb sveta

    - toggleMiniature() - cilove meritko 0.3 a zdvih 2.6 (svet jako model na stole).
    - nudgeWorld(spin, lift) - pricte k cilovemu otoceni a zdvihu (zdvih oriznuty na -3..8).
    - resetWorld() - vrati cile. Otoceni zaokrouhli na nejblizsi nasobek 360, aby se svet netocil zpet o mnoho otacek.
    - updateWorld(dt) - v miniature se svet pomalu otaci. Aktualni hodnoty se exponencialne blizi cilovym a zapisou se do getZoom, getSpin a getMove (= base transformace).

### Kyvadla

    - cyclePendulums() - rezimy srovnana / spirala / nuzky. Fazi kazdeho kyvadla nastavi tak, aby v aktualnim case melo pozadovany posun (0, rovnomerne po 360, nebo stridave 0 a 180).

### Supernova a slunce

    - supernova() - kdyz zrovna neprobiha, nastavi sunBurst 1 -> 2.4 za 0.35 s a zablesknou se krychle slunce.
    - updateSun() - stavovy automat sunPhase: 1 = zvetsovani, po dokonceni prepne na 2 = zmenseni zpet za 1.6 s, potom 0. Intenzita svetla slunce roste s druhou mocninou meritka.

### Sbirani kostek

    - startCollectibles() - odstrani stare kostky, zamicha 14 moznych pozic (std::shuffle) a na prvnich 8 vytvori zlate kostky (Kind::Collectible) s nastupem, houpanim, rotaci a vlastnim svetlem.
    - interact() na kostce: state 0 -> 1, "pop" zvetseni na 1.8, pocitadlo +1.
    - updateCollectibles() - state 1 po dokonceni prejde na 2 (zmenseni na 0.001), state 2 po dokonceni kostku odstrani (objekt, svetlo, zaznam). Po sebrani posledni kostky prijde oslava: supernova, zablesknuti a roztoceni prstence.
    - removeCollectibles().

### Drony

    - updateDrones() - kdyz dron zmeni smer (isReturning), spusti barrel roll (0.8 s, uhel 0 -> 360 se smoothstep) a "boost" (svetlo a spodek dronu se rozsviti a postupne vyhasnou). Znacka na zemi kopiruje pozici dronu.

### HUD

    - updateHud() - kazdy snimek nastavi viditelnost (H) a barvy vsech HUD prvku: aktivni prostredi sviti, sebrane kostky jsou zlate (pri oslave jiskri), telemetrie ukazuje polohu dronu na draze (getT), indikator rychlosti casu sviti podle log2(rychlost) (0.125x = 1 LED ... 4x = 6 LED), pri pauze blika cervene. Zamerny kriz je videt jen se zapnutou baterkou.

### update(deltaTime)

    - realTime - skutecny cas (pauza ho nezastavi), pouziva se pro HUD a zablesknuti. time - cas sceny.
    - updateWorld, houpani krychli prstence
    - prstenec: "boost" po kliknuti exponencialne slabne a zrychluje rotaci krychle (setSpeed). Chase efekt - pulz svetla obiha dokola (pow(cos, 6) dava ostry vrchol). Vypnute svetlo ztmavi lem i HUD vzorek.
    - reflektor - bez vybrane krychle obiha po prstenci, s vybranou krychli na ni miri (uzsi kuzel)
    - baterka - pozice a smer podle kamery
    - obihajici svetla meni barvu po duze
    - tep slunce (sunPulse) a updateSun
    - equalizer - vyska kazdeho sloupce = kruhova vlna od stredu (podle distance) + vlna zleva doprava (podle x), barva podle vysky
    - brana - tlumena pruzina: zrychleni = -60 * vychylka - 3.5 * rychlost (Euleruv krok, max 1/30 s kvuli stabilite)
    - updateDrones, updateSpawned (s deltou hodin sceny, pauza zastavi padani), updateCollectibles, updateHud, updateFlashes

### Shazovani kostek

    - spawnCube(worldPosition) - kdyz uz zije 16 kostek, nejstarsi zabije. Pozici kliknuti prevede zpet pres inverzi base do lokalnich souradnic sveta (svet muze byt otoceny nebo zmenseny). Kostka ma nahodny material, velikost, rotaci a barvu svetla a zacina ve vysce 7.
    - updateSpawned(dt) - fyzika: gravitace, pohyb, odraz od zeme (rychlost * -0.55) a "squash" (pri dopadu se kostka zplosti a rozsiri, efekt exponencialne slabne). Umirajici kostka se 0.35 s zmensuje a pak se odstrani (objekt, svetlo, zaznam).
    - kill, clearSpawned - oznaci kostky jako umirajici.

### Zablesknuti a vyber

    - flash(object) - ulozi puvodni barvu a cas. updateFlashes() barvu zesili (1 + 2.5 k^2) a za 0.6 s vrati puvodni.
    - forget(object) - pred odstranenim objektu smaze vsechny odkazy na nej (flashes, selected), aby nezustaly visici ukazatele.
    - interact(object) - kliknuti na objekt: shozena kostka = zabit, zlata kostka = sebrat, slunce = supernova, brana = rozhoupat, krychle prstence = zamerit reflektor a roztocit. Vsechno se zablesne.
    - hudClick(object) - vzorek = zapnout/vypnout svetlo, ctverec prostredi = prepnout prostredi.
    - selectNext() - projde id 1..255 od aktualne vybraneho a vybere dalsi existujici objekt.
    - printSelection() - vypise id, druh, pozici (posledni sloupec matice transformace), barvu a pres dynamic_cast i popis kompozitu (describe, size).
    - replayIntro() - restart vsech nastupovych animaci a srovnani kyvadel.
    - toggleFlashlight, toggleChase, toggleHud.

## Vztah k ostatnim tridam

    - Scene - predek, vsechny objekty, svetla a transformace vytvari pres jeji tovarni metody.
    - pouziva vsechny transformace (Translate, Rotate, Scale, CustomMatrix, DynamicRotate, Oscillate, LineMove, GrowScale, HudPlacement, TransformationComposite), Light, Material a Resources.
    - ShowcaseInput - vola jeji verejne metody.

## Navrhove rozhodnuti

    - Jeden model, mnoho transformaci - ukazka, ze slozitou scenu jde postavit jen z krychle a skladani transformaci.
    - Sdilene dynamicke transformace - planeta a mesic, svetlo a jeho znacka, cela sroubovice: jedna animace ridi vic objektu.
    - Dekorativni objekty jako staticke - setri omezena id pro vyber.
    - Cas sceny vs. realny cas - animace sceny respektuji pauzu, UI efekty (HUD, zablesknuti) ne.

## Na co si dat pozor

    - Pocet vybiratelnych objektu je blizko limitu 255 (zhruba 150 objektu sceny + 45 HUD prvku + 8 sbiratelnych + az 16 shozenych kostek). Dalsi objekty je lepsi pridavat jako decorative.
    - Pocet svetel: 22 stalych + 8 sbiratelnych + az 16 kostek, limit je 64.
    - Struktury drzi ukazatele na objekty a svetla vlastnene Scene. Pri odstraneni je nutne zavolat forget() a vymazat zaznam (v kodu je to dodrzeno).

# TestScene

    TestScene je prazdna testovaci scena. Slouzi jako cisty zaklad pro zkouseni novych veci a jako ukazka minimalni sceny.

## Popis implementace

    - class TestScene : public Scene
    - using Scene::Scene; - zdedi konstruktor Scene(Resources&), neni potreba psat vlastni.
    - create() override - kamera do pocatku se smerem -Z (alpha 90, phi -90), odstrani vychozi podlahu (removeGround), vypne skybox a nastavi cerne pozadi. Vysledkem je uplne prazdna scena.
    - update(deltaTime) override - prazdne.

## Vztah k ostatnim tridam

    - Scene - predek.
    - TestSceneInput - jeji ovladani.
    - Application - pridava ji jako druhou scenu (TAB).

# Application

    Application je vrchol celeho programu. Inicializuje GLFW, okno a OpenGL, vlastni Resources a vsechny sceny, bezi hlavni smycku, preposila udalosti z GLFW do vstupu aktivni sceny a na konci vse ve spravnem poradi uklidi.

## Popis implementace

    - struct SceneEntry - dvojice unique_ptr<Scene> a unique_ptr<SceneInput>. Scena a jeji ovladani patri k sobe.
    - addScene<SceneT, InputT>() - sablonova metoda: vytvori scenu s Resources, zavola build() (az tady se zavola create potomka), vytvori ovladani a ulozi dvojici. Pridani nove sceny je jeden radek v createScenes.
    - privatni promenne: window, resources (unique_ptr), scenes, activeScene, mainInput{ *this }, posledni pozice kurzoru (pro vypocet dx, dy), stav fullscreen a ulozena pozice a velikost okna (pro navrat z fullscreen).
    - createScenes() - prida ShowcaseScene + ShowcaseInput a TestScene + TestSceneInput a aktivuje prvni scenu.
    - initialization():
        - glfwSetErrorCallback, glfwInit (pri chybe konec)
        - window hinty: OpenGL 3.3 core profile, forward compatible (nutne na macOS), 8bitovy stencil buffer (potreba pro vyber objektu)
        - vytvoreni okna 800x600, glfwMakeContextCurrent, glfwSwapInterval(1) = V-Sync
        - gladLoadGL nacte funkce OpenGL (pri chybe uklid a konec)
        - vypis verze OpenGL, vyrobce, rendereru, GLSL a GLFW
        - viewport podle velikosti framebufferu
        - glEnable(GL_DEPTH_TEST), glEnable(GL_STENCIL_TEST), glStencilOp(KEEP, KEEP, REPLACE) - kdyz pixel projde testy, zapise se do stencilu referencni hodnota (id objektu z DrawableObject::draw)
        - glfwSetWindowUserPointer(window, this) a registrace callbacku
        - pocatecni pozice kurzoru
        - vytvoreni Resources - az ted, protoze potrebuje OpenGL kontext
    - run() - hlavni smycka, dokud okno nema byt zavreno:
        - deltaTime = rozdil glfwGetTime() mezi snimky
        - input->update (pohyb kamery), scene->tick (hodiny + animace), barva pozadi ze sceny
        - glClear barvy, hloubky i stencilu
        - scene->draw() a hned potom input->processPendingClick (framebuffer obsahuje aktualni stencil s id)
        - glfwSwapBuffers, glfwPollEvents
        - po smycce uklid ve spravnem poradi: nejdriv sceny (drzi ukazatele na zdroje), pak Resources, pak okno a glfwTerminate. Vse s GPU zdroji se musi smazat, dokud OpenGL kontext existuje.
    - active() - ukazatel na aktivni SceneEntry nebo nullptr.
    - activateScene(index) - nastavi aktivni scenu a zavola activate s aktualni velikosti framebufferu (scena mohla byt neaktivni, kdyz se menila velikost okna).
    - switchScene(step) - cyklicke prepinani: ((aktivni + krok) % pocet + pocet) % pocet. Druhe pricteni poctu resi zaporny krok (Shift + TAB).
    - close() - glfwSetWindowShouldClose, smycka skonci.
    - toggleFullscreen() - pri prechodu do fullscreen ulozi pozici a velikost okna a nastavi okno na primarni monitor v jeho rozliseni. Zpatky obnovi ulozene hodnoty.
    - Callbacky GLFW jsou static, protoze GLFW je knihovna v C a neumi volat metody objektu. Proto from(window) vytahne ukazatel na Application ulozeny pres glfwSetWindowUserPointer.
        - keyCallback - nejdriv MainInput, kdyz klavesu nezpracuje, posle ji vstupu aktivni sceny
        - mouseButtonCallback, scrollCallback - preposlou aktivni scene
        - cursorPositionCallback - GLFW posila absolutni pozici, prevede se na rozdil (dx, dy) od minule pozice
        - framebufferSizeCallback - glViewport a resize aktivni sceny
        - errorCallback - vypis chyby GLFW
    - #define GLAD_GL_IMPLEMENTATION - v tomto souboru se vygeneruje implementace GLADu (musi byt prave v jednom .cpp).

## Vztah k ostatnim tridam

    - vlastni Resources, vsechny Scene a SceneInput a MainInput.
    - main.cpp - vytvori Application a zavola initialization, createScenes a run.

## Navrhove rozhodnuti

    - Kazda scena ma svou kameru, hodiny a objekty, ale vsechny sdileji Resources. Prepinani scen je okamzite a zdroje se nenacitaji znovu.
    - Udalosti jdou retezem MainInput -> SceneInput (Chain of Responsibility).
    - Neaktivni sceny se neaktualizuji ani nekresli, jejich cas stoji.
    - Poradi uklidu je explicitni (scenes.clear(), resources.reset()) a nespoleha se na poradi destrukce clenu.

## Na co si dat pozor

    - V SceneEntry je input deklarovany po scene, takze se nici drive nez scena (drzi na ni referenci). To je spravne.
    - Zmena velikosti okna se posila jen aktivni scene. Ostatni sceny se aktualizuji az pri aktivaci (activateScene).

# main.cpp

    Vstupni bod programu. Je zamerne co nejkratsi, veskera logika je v Application.

## Popis implementace

    - Application app; - vytvori aplikaci na zasobniku (destruktor se zavola automaticky na konci main).
    - app.initialization(); - GLFW, okno, OpenGL, Resources.
    - app.createScenes(); - vytvoreni a sestaveni scen.
    - app.run(); - hlavni smycka, vrati se az po zavreni okna (vcetne uklidu).
    - return 0;

## Navrhove rozhodnuti

    - Poradi tri kroku je dulezite: sceny potrebuji Resources a OpenGL kontext, smycka potrebuje sceny. Rozdeleni do tri metod misto jednoho konstruktoru dela poradi viditelne.