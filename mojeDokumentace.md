# BASICMODELS
zde jsou ulozeny modely ktere jsou primitivni jako napriklad triangle, square, ground, cenered triangle, skycube a texturedFloor

# TRANSFORMATION
Abstrakni trida, ze ktere dedi vsechny transformace, implementuje virtualni desktruktor a virtualni metodu getMatrix.

# TRANSLATE
Translate dedi z transformation, ma naimplementovany konstruktor, ktery prijma vec3 coz je ten offset o ktery se ma model posunout. Getmatrix se zde implementuje a setOffset a getoffset jsou jen gettery a settery privatnoho offset.

pokud zavolame getMatrix, zavola se glm::translate na offset a tak se nam vrati matice posunuti.

# ROTATE
podobne jako translate dedi z transformation, mame zde konstruktor, kteremu dame uhel ve stupnich a okolo jake osy se rotace provadi. setangle a get angle jsou settery na privatni angleDeg

getMatrix vrati glm::rotate s prevedenymi deg na radiany a axis.

# SCALE
scale opet dedi z Transformation. Mame zde dva konstruktory a to, ze muzeme model zvetsit podle vektoru a nebo ho zvetsit o skalar. setfactor a getfactor jen nastavuji a vraceji privatni factor.

getmatrix vraci glm::scale kde se matice jen prenasobi factorem

# CUSTOMMATRIX
custommatrix dedi z trasformace, ma privatni glm::mat4 a verejny Custommatrix a metody getmatrix. Obaluje glm::matrix tak, at jde pouzit v Transformationocmposite

# DYNAMICROTATE
Dynamic rotate take dedi z Transfromation a slouzi pro rotaci v case. DegPersoceond je o kolik stupnu se kazdou sekundu rotace provede, axis opet na jake ose s provede.
getangle, setSpeed a getspeed manilujuji a vraceji privatni hodnoty. lstartangle je dan na 0.0 stupnu.

getAngle vraci aktualni uhel natoceni ve stupnich, pociuta ho z uplynuleho casu a rychlosti. glfwGetTime vrecu pocet sekund od spusteni glfw a startime je casova znacka, kdy se rotace naposledy rozbehla, tedy v konstruktoru nebo setSpeed. Rozdil je tedy doba, po kterou se obekt toci od chvile kdy mu byla tahle konkretni rychlost zmenena nebo nastavena.

setSpeed nastavi startangle na momentalni angle, start time na momentalni cas a degparsecond na parametr funkce.

getmatrix vraci glm::rotate kde je getangle() preveden na radiany.

# GROWSCALE
Growscale je opet transformace co dedi z Transformatin, mame privatni promene target, duration a startTime

GrowScale postupne zvetsuje objekt znuly na konecnou velikost.
v konstruktoru se ulozi startTime na glkfwGertTime
v GetMatrix se spocita cas rustu jaka uz probehla; x = (sktualni cas - startTime) / duration; toto je omezeno na interval <0;1>

hodnota se prevede krivkou smoothstep : x^2 * (3 - 2x). rus takzrychluje a pak zpomaluje a na konci pristane bez trhnuti.

vysledna matice je rovnomerne meritko target * eased.

getMatrix prave toto omplementuje. Vezmeme x, coz je ten (cas - starttime) / duration v sekundach. pak se to prevede na ten interval <0 ; 1>. eased nam udela ten efekt zpomalovani bez trhnuti a potom se jen pouzije glm::scale kde misto target dame target * eased.

# LINEMOVE
Linemoved je dedi z transformace a pomaha nam interpolovat pozici mezi 2 body. v konstruktoru se vezme bod a a bod b mezi kterymi bude obekt pendlovat a jak rychle ma mezi temito body behat.

getMatrix vrati matici transformacem GetT vraci cilso v intervalu <0; 1>a toto cislop znaci, kde na primce se objekt nachazi, 0 = bod A a 1 = bod B, takze desetine cislo naprklad 0.5 je presne v polovine. neni to tedy skutecna vzadalenost a el informace o tom, v jake casti vzdalenosti se nachazi, jestli se nachazi na zacatku, na konci, uprostred, v jedne tretine nabop kdekoli jende.

getPosition vraci pozici ve vectoru.

GetT nejdrive spocita delku pomoci glm::length a to vrati float length
pokud je length blizko nule, proste vratime nulu. Jinak by jsme v dalsim kroku delili nulou a to nam k nicemu neni.
pak vypocitame S a to je pocet prejeti usecky 0 = A, 1 = B, 2 = A, 3 = B atd...
no a vratime kde se momentalne nacahzime std::fmod(s, 2.0) vrati zbytek po deleni dvbema, tedy hodnotu v rozsahu 0 az 2, 0, 2 = A, [0;1] je cedta tam, 1 = B, [1;2] je cesta zpet. 1.0 - x posune stred na 1, pro x od 0 do 1 je vysledek kladny a pro 1 do 2 je zaporny. std::fabs tento vysl;edek vezme a udela z neho vzdy kladny, takze vznikne vzdy vzdalenost od 1. no a 1.0 - ... to otoci a vysledek je 0 na zacatku a 1 uprostred cyklu, zase 0 na konci a tak dokola...

getPosition jen vezme t bod (1.0 - t) A + t * B coz je vlaste bod mezi temito body.

getMatrix vezme pozici prave vypoctenou z GetPosition a provede posun pomoci glm::translate

# TRANSFORMATIONCOMPOSITE
TransformationComposite zavrsuuje vsechny transformace a take dedi z transformace. Mame zde add static a addDynamic a nebo proste jen Add.
add a addDynamic se lisi v tom, ze addDynamic ulozi jen ukazatel na transformaci, pri kazdem getMatrix se zavola t->getMatrix() znovu, proto na nej pusobi cas napriklad DynamicRotate, Linemove nebo GrowScale. Objekt na ktery ukazuje musi zit jinde v Application. addTransformation je inline fonkce, ktera vola addDynamic, chovani je uplne styejne ale je tojen zkratka. AddStatic zavola t.getMatrix jednou hned pri pridani. Vysledek zkopiruje do vlastni staticMatrix, kterou comosite sam vl;astni. Kdy+z za sebou pridame nekolik statickych transformaci, composite je slouci do jedne matice vynasobenim, usetri se tak misto v seznamu, pokud do addStatic same dynamickou transformaci, zamrzne na hodnote v okamziku pridani.

v private vytvorime class StaticMatrixm ktera do sebe uklada ty staticke transformace.

addStatic vezme ukazatel na transformaci a udela si kopii jeji matice, dale zkontroluje jestli na konci vektoru transformaci je take staticka a pokud ano, tak se posledni prenasobi toutou., jinak se vytvoriu na konci statickaMatice a naplni se tou z transformation.

u addDynamic se prioste transformace pushne nakonec

getMatrix vrati cely vektor matic prenasobeny vsemi ve spravnem poradi

describe proste vypise jak je vektor usporadany, napriklad S D D D D S D , nikdy by se nemelo stat, ze jsou dve S bezprostredne vedle sebe, D vedle sebe byt muzou a je to spravne S = staticka a D dynamicka

# ICAMERAOBSERVER
Icamera observer je observer na camru, mame zde doprednou deklaraci kamery a delault virtualni destruktor, a metoru onCamereaChanged.

# CAMERA
udelame si doprednou deklaraci ICameraObserver a definujeme Camera, kameru muzeme k nejake class pridat a tim bude prijmat informace o tom, kdy se kamera zmenila.
noitifyAll bude informovat vsechny pripojene class o tom, ze se neco zmenilo.

zakladni rozlozeni je: 
alpha = 90
phi = -90
fov = 60
aspect = 4 / 3
nearPLane = 0.1
farPlane 100
speed = 1.5
sensitivity = 0.15
eye = (0, 0, 2)
target = (0, 0, -1)
up = (0, 1, 0)

kamera je ke objekt, kteryu drzi pohled, kde ja oko, kam se diva a jak siroky obraz vidi, z techto hodnot pocita dve matice a to projectionMatrix a viewMatrix. tyto matice pak dostanou shadery. Observer je navrhovy vzor, kde camera je subject a ICamera observer je rozhrani.
attach() - jamera si zapamatuje, kdo ji sleduje a prida ji do vektoru observers. NotifyAll zase u vsech observers spusti onCameraChanged. Vola se po kazde zmena a to po zmene pohybu, otoceni, zmeny FOV a nebo i pomeru stran.

v Application se kamera pripoji ke vsem shaderum. A kdyz se kamera pohne, shadery dostanou nove matice view a projection a nemusi si to hlidat samy. Kamera protom vubec nemisio vedet, co je shader, takze obe casti jsou naprosto oddelene.

smer pohledu, neboli target se pocita z dvou uhlu v updateTarget()
x = sin(alpha) * cos(phi)
y = cos(alpha)
z = sin(alpha) * sin(phi)
kde alpha je uhel osy y 90stupnu znamena vodorovny pohled, omezuje se na 1 az 179 aby se kamera neprevratila pres pol. Phi je uhel kolem osy y. prave tlacitko mysli meni phi a alpha.

eye = pozice oka ve svete, kamera tedy stoji 2 jednotky od poocatku na ose z (0, 0, 2)
target = (0, 0, -1) jednotkovy vektor, ktery reprezentuje smer a ne jako bod.
up = je smer nahoru, podle nej se pociuta pravy vektor a lookAt. ten se nemeni.
alpha = je to uhel od svisle osy z neja se pocita sklon pohledu nahoru a dolu
phi = uhel okolo svivle ody, znej se pocita otoceni doprava a doleva 
fou = FIELD OF VIEW 60 je vertikalni uhel zaberu ve stupnich,vetsi cislo = sirsi pohled a vice ribi oko.
ascpect = pomer sirky a vysky okna
nearPlane = nejblizsi vzdalenost, kterou kamera vykresli
farPlane = nejdelsi vzdalenost co sevykrasli
speed = rychlosypohybu v jednotkach za sekundu (WASDQE)
sensitivity = kolik stupnu se kamare otoci na jeden pixelk pohybu mysi.

# LIGHT
Trida Light drzi data o svetelnem zdroji, Sama nic nekresi, jejio hodnoty pak Scene::draw preda shaderum jako uniformy.
position je pozice svetla, kde vychozi je pocatek (0, 0, 0) ale do sveta se dostane az pres treansformaci sceny.
Color je barva svetla, vychozi je bila (1, 1, 1), nasobiu se difuzni a zrcadlovou slozkou materialu. Ambiantni slozka se ji nenasoibu. v phgong.frag je ambient = ,aterial.ambient * baseColor. 
constatnt, linear a quadratic jsou koeficienty utlumu, tedy jak rychle svetlo slabe se vzdalenosti.

utlum shader spocita intenzitu podle vzdalenosti d od sveta = att = 1 / (constant + linear * d + quadratic + D^2) vysledek se omezi na rozsah 0 az 1 a kazdy clen ma jinou roli: Constant je zaklad a zabranuje deleni nulou a dava maximalni intenzitu blizkeho svetla, Linear slabne linearne sse vzdalenopstii a dominuje na stredni vzdalenosti a quadratic slabne s druhou mocninou, dominuje na velke vzdalenostu a odovida fyzice bodoveho zdroje.

Pro vychozi hodnoty (1, 0.09, 0.032) vychazi ze:
vzdalenost:         intenzita (attuation):
             0  1.00
             1  0.89
             5  0.44
            10  0.20
            20  0.06

pokud se nastavi setAttenuation(1, 0, 0), utlum se vypne a svetlo ma vsude stejnou hodnotu. Tak to je napriklad slunce.

# MATERIAL
mterial ma vsobe ambient, diffue a specilar jako vec3, take shininess a metodu apply

material p;opisuje, jak povrch objektu odrazi svetlo, kazdy objekt muze mit vlastni material, takze dve koule se stejnym svetlem mohou vypadat jinak.

mame zde 4 polozky:
    ambient je vychozi 0.1 a zadava jak moc povrch odrazi okolni svetlo, tato slozka nezavisi na pozici svetla, takze zajistuje, ze objekt neni uplne cerny ani ve stinu, Tmave mistiov nocni scene ma ambient 0.02
    diffuse je vychozi 1.0 a zadava, jak moc povrch odrazi svetlo rozptylene, Matny povrch, ktery vypada stejne z kazdeho uhlu ma hlavne jen difuzni slozku
    specular ma vychozi 1.0 a je to barva a sila zrcadloveho odlesku. Hodnota 0 znamena, zadnyt lesk a vyssi hodnoty naopakk znamenaji vyraznejsi odlesk
    shininess mam vychozi 32 a je to exponent, ktery urcuje, jak ostry je odlesk. Mala hodnota dava valky a rozmazany odlesk velka naopak maly a ostry bod.

vsechny hodnoty posliame do shaderu jako uniformy, jen pokud je shader podporuje.

# SHADER
shader nahrajeme pomoci cesty, ma typ, tedy vertex nevbo fragment shader.
delame zde nacteni ze souboru, a tvoreni shaderu

# SHADERPROGRAM
linkuje shadery dojednoho shaderProgramu, vezme fragemnt a vertex shader a da je do jednoho. podorujeme uniformy int, float, vec3 a mat4. Dedi z Icameroobserver, takze musime naimplementovat onCameraChanged.

cache pozic nam pomaha, protoze glGetUniformLocation je po,ale a prtoto se kazdy vysledek pro kazde jsmeno ulozi do unoirdered_map. slovoi mutable umoznuje ukladat do cache i z const funkcio jako je hasUniform.
HasUniform rika, jestli shader dany uniform ma. Pomaha to nenastavovat prebytecne uniformy, kdyz ho shader stejne nema. to se stane, kdyz v glsl definovany je, ale pouzity neni, glsl ho automaticky vyhosi a proto i kdyz v shaderu je jako uniform, nemusi tam byt fyzicky.

onCameraChanged nastavi uniform viewMatrix a projectionMatrix a pripadne cameraPosition.

# TEXTURE
Textura je obrazek co se da misto barvy, model musi znat U a V. Pkud chceme pouzit texturu, nejdrive nacteme obrazek z disku, nahrajeme ho na grafickou kartu a pripravime ho pro sader jako texturu.
stbi_set_flip_verticaly_on_load(true) nam zajisti ze nebudeme mit problem s loadovanim obrazku: obrazky se totiz ukladaji s pocatkem vlevo nahore (0, 0)m ale opengl pocita souradnici V od spodniho okraje, Tento prikaz obrazek pri nacteni prevrato aby (0, 0) v UV ospovidalo levemu dolnimui rohu, JE to glovalni nastanevi, takze plati pro vsechna dalso nacteni dokud se nezmeni.

unsigned char* pixels = stbi_load(path, &width, &height, &channels, 4) posledni parametr 4 vynutu ctyri kanaly RGBA i ksyd je soubor RGB nebo  sedotonovy. Shader tak vsdy dostane stejny format.

Parametry textury:
    GL_REPEAT - souradnice mimo rozsah 0 - 1 se opakuji, diky tomu se drevinarta podlaha muze vykreslit jako opakujici se dlazdice
    GL_LINEAR_MIPMAP_LINEAR - pri zmensovani se interpoluje mezi pizely i mezi urovnemi mipomap, rika se tomu trilinearni filtrovani.
    GL_LINEAR - pri zvetsovani se interpoluje mei pixely.

# TEXTUREMANAGER
Je to unordered_map obal pro textury. Dunguje jako cache texturm zajistuje ze stejny obrazek nenacte a nenahraje na GPU dvakrat. Textury tak sdili vice objektu a nic se neduplikuje.

# MODEL
Reprezentuje model, jeko vbo a vao

# MODELLOADER
ModelLoader nacte 3D model ze souboru jako OBJ, FBX a dalsu formaty pres assimp a prevede ho na data, ktere umi opengl vykreslit.

MOdelPart popisuje jednu cast modelu, tedy jedu sit s jednim materialem, ma v sobe vrcholova data, cestu k difuznmi texture ktera muze byt prazdna a barvu materialu, kdyz textura chybi.

LoadedModel je cely nacteny soubor, ma vsechny modelParts a bounds, tedy roky kvadru kolem celeho modelu.
MOdelLoader nema zadna data, a ma jen statickou funkci load(path), ktera vrati LoadaeNodel.
v Application se z kazde casti presude model do ektoruy models, vytvori se DrawableObject s touto casti a pokud je texturePath na disku, ziska textura pres TextureMaager. Jinak se pouzije diffuseColor. z boundsMina voundsMax se spocita meritko a posun.

# CUBEMAP
je to textura slkozana z sestai ctvercovych obrazku, jeden por kazdou stenu kdychle +X, -X, +Y, -Y, +Z a -Z. POkud si predstavime, ze kamera je uvnitr krychle a kazda stena je jeden obrazek, uvidimeoklni krajinu ze vsech smeru,
Shader nacte souradnice (u, v) ale smer jeko trojrozmerny ketor texture(skybox, direction) vybere stenu podle nejvetsio slozky smeru a na ni najde pixel. Trik je v tom Ze to obejde depthBuffer a tak se zda, ze pozadi je velice daleko.

# SKYBOX
Skybox jen vezme Cubemap, shaderProgram a model Cube a da je dohromady a prida jim vlastnosti.

Vypne DepthMask
pouzije shaderProgram
Pripoji cubemapu na texturovou jednotku 0
da programu uniform 0
vykresli krychli 
a opet zapne depthMask

# DRAWABLEOBJECT
Drawable object je jedna vec ve scene, Spojije vsechno co je potreba k vykresleni, geometrii, shader, polohu, vzhled a identitu pro kliknuti. sama nic nevlastni, jen drzi ukazatele na objekty ktere si vytvoRila Application

model je geometrie
program je shaderprogram
transformation je TransformationComposite (nemusi byt ale skoro vzdy je, jinak tomuze byt cokoliv, co z transformation dedi)

color je vec3 barvy
material jsou parametry osvetleni ambient, siffuse, specular a shinines
texture je obrazek na povrchu
id je cislo zapsane do stencilBufferu, po prekroceni 255 je 0
koind je druh objeku (bezny, zeme, mince a pohubliby)

objekt nema vlastni stav na gpu, pri kazdem, vykresleni nastavy hodnoty uniformu ve svem shaderu a pakl zavola kresleni
Nejdriv resinem polohu
pak BARBY
pak MATERIAL
pak TEXTURU
pak se zaktivuje shader
stencil 
a nakonec model->draw()

po vykresleni cele sceny se glReadPixel precte stencil hodontu pod kurzorem. Ta je id objektu,ktery jde videt nejblize a SCENE::getObjectById(id) podle ni najde DarawableObject.

Kind rika, jak se objekt chova pri kliknuti v handleClick:
    Normal - vybere se a zcervena
    Ground - ignoruje se ale v rezimu insert je to misto pro vlozeni stromu
    Collectible: odstrani ze sceny a zvysi pocitadlo
    Moving: odstrani ze sceny

# SCENE
Scene je jeden svet, ve kerem se vse zobrazuje najednou, kazda scena v Application ma vlastni kameru, svetlo, oblohu, seznam objekyu a spolecnou transformaci pro ovladani. Pri prepnuti sceny se zobrazi jiny Scene a ostatni se nevykresluji.

objects jsou dynamicke objekdt, ktere dostanou id v add, takze na ne jde kliknout a vykresluji s kazdy snimek.
statiocObjects jsou objekry pridane pres assStatic, ty nedostanou Id, takze na ne kliknout nejdem pouziva se pro pevne prvky, napriklad na login.

NextId zacina na 1 a 0 znamena pozadi
freeIds je zasobnik uvolnenych cisel, kdyz se objekt odtranim jeho cislo se vrati a znovu pouzije,
stencil buiffer ma 8 bitum takze je k dispozici 255 cisel, vice objektu uz oznacit nejde.

camera a light jsou vlastni pro kazdou scenu
litPrograms jsou shadery, ktere potrebuji data o svetlen pri vytvoreni sceny je naplni APPLICATION::createScene
lightTransformatiuon je bolitelna transformace, ktera pohybuje svetlem. Pouziva ji nocni les kde svetlo sedi na svetlusce.

clearColor je barva pozadi a skybox je obloha pokud je nastavena.

controlMove (posun), contorlSpin (otoceni)a controlZoom jsou ovladane klavesami sipek, r a +/-
base je TransformationComposite,ktery tyto tri drzi jako dynamicke potomy v tomto poradi.

vkonstruktoru se do base pridaji tri ridici transformace, Kdyz AppplicationL::makeComposite vytvori transformaci pro objekt, jako prvni potomka preda prave scene->getBase() vysledna matice objektu je proto: (M = base * vlastni transformace objektu). Diky romu se cela scena posune, otoci a priblizi najednou objekty maji vlastni pozici uvnitr ni.

vyjimka je na insertModel. Ktery vytvari transformaci pres make<Transformatiocomposite>() bez base. Vlozeny Objekt ma souradnice sveta z unProject. Takze se se scenou hybat nema.

add(object):
    oikud he ve freeIds volne cislo9, pouzijese,jinak se vezme nedtId.
    kdyz je vice nez 255 objektu, vypise se varovani a objekt nepujde vybrat
    setId(id) a objekt se prida do objects

remove(object):
    Najde objekt ve vektoru, pokud tam neni, vrati false
    odstrani ho ze seznamu,jehoi cislo vrati do freeIds a nastavi mi id na 0;

getObjectById(od) projhde objects a vrati objekt s timto cislem, pro id = 0 vrati null protoze je to pozadi (a nebo objekt ktery presahl 255 a neniu pro nej volne id)

draw()
    Obloha: pokud existuje, nastavi se stencil na 0 vaykresli se, Skybox::draw
    Svetlo: svetlo se pocitra jednou za snimek pro celou scenu ne pro kazdy objekt zvastm protoze jde pres base, pohybuje se i se sipkami
    staticke objekty: kazdy se vykresli pomoci DrawableObjects::draw(), nemaji ID, takze se do stencilu zapise 0
    Dynamick eobjekty, stejny postup jako u staticm ale kazdy nastavi svoije uniformy a vykresli se, pokud prekrybvaji jiny objeky, ten je schovany diky z-bufferu, ne diky poradi kresleni.
    odpojeni: ShaderProgram::unbind() zavola glUseProgram(0), aby dalsi kod nepouzil posledni shader omylem

# APPLICATION
je to hlavni trida programu, vytvari okno, shadery, modelym a sceny a spousti hlavni smycku.

initialization() - vytvori kokno, nastavi opengl, stancil a callbacky klavesnice a mysi
craeteShaders() - vytvori vsaechny shadery
createModels() - vytvori vsechny modely
run() - hlavni smycka, zpracuje vstup, aktualizuje hodiny , vykrasli aktovni scenu, zpracuje klik a prohodi buffery.

GLFW vyzaduje obycejne funkce, ne metodu, proto jsou callbacky (keycallback, cursorPositioncallback a dalksi) static. Ukazatel na Application si berou pres glfwGetWindowUserPointer a pak volajki jeji nestaticke metody

enum class InteractionMode {
    Select,
    Insert,
    Path
} jsou tri rezimy lebeho tlacitka, Prepinaji se klaveou F2, nebo M

handleClick()
 cte pixel pod cursorem a podle rezimu voila jedu z ostatnich funkci
 selectObject oznaci object cervenycm odstinem
 clearInteracttion zrusi vyber i rozpracovanou cestu
 insertModel vlozi strom nebo shreka

 processInput(deltaTime) cte klavesy wasd, spiky, +/-, r a posouva kameru a riditransformace
 updateClock() nastavy uhgly rucicek podle skutecneho casu
 activateScene(index) prepne aktoivni scenu a upravi kameru

Sablona make<T>
vytvori sablonu typy T pres unique_ptr, ulozi ho do vektoru transformations a rati obycejny ukazatel. tim se ziska pohodlne vytvareni transformaci bez rucniho hlidani zivotnosti, vlastnikem zustava Applkication.

createProgram zkompiluje shader a ulozi ho do shaderPrograms
makeComposite(scene) vytvori transformationcomposite a jako prvniho potomka prida zaklad sceny neboli BASE
createObject vytvori DrawableObject a ulozi ho do onjects
createScene vytvori scenu, pripoji k jeji kamera shadery a nastavi pomer stran
assSignature prida scene lohgin
addClock prida hodiny s rucickama

## stavitele scen
kazda demonstarace z cviceni ma vlastnbi funkci

### CreateForestScene
    les se stromi, hodinamu a sberatelnymi mincemi
### CreateClockScene
    samostatne hodiny
### CreateSolarSystemScene
    sl;unce, zeme, mesic a raketa s hierarchili transformaci
### CreateOrderScene
    poradi transformaci
### CreateModelMatrixScene
    slozeni, posun otoceni, meritko
### CreateLoginScene
    text s logine,
### CerateSphereScene
    ctyri koule se ctyrmi typy osvetleni
### CreataNormalScene
    normalya normalova matice
### CreateBackSpecularScene
    odlesk naodvracene strane
### CreateNightForestScene
    nocni les se svetluskou jako svetlem
### CreateShrekScene
    model shrek s texturou
### CreateTextureScene
    podlaha, shrek, fiona a stoilet
### CreateClassRoomScene
    uceba
### CreateTerrainScene
    teren

## nacitani
    pathOrPlaceholder(path,placeholder) - pokud soubor neexistuje, pouzije se zalozni soubor, diky tomu program bezi i bez assetu.
    LoadObject(...) - nacte model pres ModelLoader, spocita meritko a posun, vytvori objekty a vrati boundaries
    getSkybox() - vytvori oblohu jen jednoiu a vraci ji opakovane
    setSpheresProgram - nastavi shader vsem ctyrem koulim

## nastaveni a klavesy
    funkce cycle... toggle... switchScene, printHelp a printStatus jsou sdilene mezi posmenkovymi kl;avesami a F1 az F12, SHIFT je obraci.

## Clenove tridy
    OKNO A KURZOR: window, lastCursorX, lastCursorY
    SHADERY
    MODELY
    ATD...

## --- IMPLEMENTACE ---

Sceny se v kodu vytvareji v poradi 1 az 0 je prepinaji, pokud ma scena index vetsi nez 0 (to znamena nemame tlacitko vice napravo nez 0 ne ze pokud je scena > 0) daji se prepinat tabem ci shift+tab pro scenu pred.

v createShaders, vytvorime vsechny ShaderProgramy jako:
colorProgram
normalProgram
treeProgram
bushProgram
clockProgram
hudProgram
constantProgram
lambertProgram
phongProgram
blinnProgram
nightTreeProghram
nightBushProgram
skyboxProgram

nasledne nekterym z nich nastavime uniformy

do cameraPrograms pridame programy, ktere potrebuji matice pohledu a projekce
do litPrograms pridame programy, ktere potrebuji data o svetle

makeComposite vytvori Transformationcomposite a jako prvni do jen da scene->getBase(). Vysledkem je, ze vsechno cve scene se pokybuje a otaci spolecne ovladanim sipek a klavesy R. Vl;astni transformace objektu se pak pridavaji az ZA zaklad.

createObjects vytvori Drawable object a ulozi ho do objects.

createScene je zaklad kazde sceny: vytvori scene, a ulozi jej do scenes, pripoji vsechny camera programs ke kamere jako pozorovatele a prida vsechny litPrograms do scenyt aby dostavalu data o svetle a nakonec nastavi pomer stran podle velikosti okna a zavola NotifyAll, takze shadery dostanou matice.

Nasleduje vytvoreni scen...


pethOrPlaceholder je pomocna funkce, kterou jsem jiz popisoval

# MAIN

# VERTEX
## clock.vert
## foliage.vert
## lit.vert
## litFoliage.vert
## model.vert
## skybox.vert

# FRAGMENT
## constant.frag
## normal.frag
## color.frag
## skybox.frag
## lambert.frag
## blinn.frag
## phong.frag
