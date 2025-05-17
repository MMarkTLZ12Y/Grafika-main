# Noé Bárkája - Resource Collection Game

## Projekt leírása

Ez a játék egy "Noé bárkája" témájú erőforrás-gyűjtögetős játék, ahol a játékos célja, hogy összegyűjtse és visszahozza a megfelelő mennyiségű alapanyagot, állatokat és a feleségét a pálya közepén található házba minél gyorsabban.

A játék során fák, állatok, a feleség és szurok gyűjtése a cél, valamint ezek visszaszállítása a központi házhoz.

---

## Játékmenet és funkciók

- **Játékos célja:**
  - Megnyerni a feleséget (egy a pályán véletlenszerűn létrejött csokor virág által)
  - Összegyűjteni 2×3 db állatot (véletlenszerűen mozgó entitások)
  - Összegyűjteni 30 db fát (véletlenszerűen szórva a fák alatt)
  - Előállítani 10 db szurkot, amihez 20 db fa elégetése szükséges a ház melletti tűzrakóhelyen (1 db szurokhoz 2db fa kell. Minden szurok elkészítése 20 másodperc időtartamú folyamat)

- **Gyűjtés és mozgatás:**
  - A játékos a házból indul, és oda kell visszajuttatnia a begyűjtött erőforrásokat, állatokat és a feleséget.
  - A fák véletlenszerűen helyezkednek el a pályán a fák alatt.
  - A feleség véletlenszerűen jelenik meg a pályán, a csokor virággal tudja a játékos „megnyerni”. Ezt követően magától elindul a házhoz. (A csokorvirág is véletlenszerűen jelenik meg a pályán)
  - Az állatok lassabbak a játékosnál, de véletlenszerűen mozognak a pályán. Ha a játékos nekik ütközik, az adott állat automatikusan a házhoz megy.
  - Egyszerre legfeljebb 7 db fát tud a játékos magánál tartani.

- **Tűzrakóhely és szurok készítés:**
  - A ház mellett található tűzrakóhelyen 2 db fát lehet elégetni egyszerre.
  - Az égetés 20 másodpercig tart, és ennek végén 1 db szurok keletkezik.
  
- **Kamera és nézet:**
  - A játék egy FPS-szerű kameranézettel fut, ahol a játékos saját karakterét nem látja.
  
- **Grafika és modellek:**
  - Az állatok, fák és a feleség beöltött `.obj` modellek.
  - A ház és a tűzrakóhely textúrázott elemek.
  - A tűz animációval ég a szurok égetésekor.

- **Segítség:**
  - `F1`: Súgó – megjeleníti, melyik gomb mit csinál.
  - `F2`: Hints – további játékmenetbeli tippek megjelenítése.

---

## Telepítés és futtatás

A játékot C nyelven készült SDL2 és OpenGL használatával fejlesztettem. 

A futtatáshoz szükséges:

- Telepített SDL2 és OpenGL környezet
- A program forráskódjának lefordítása és futtatása

---

## Játékvezérlés

- Mozgás: WASD
- Interakció: E - szurokégetés elkezdése
- Kamera: Egér mozgatás (egyszerű FPS nézet)
- Menü/Súgó: F1, F2

---

## Játékélmény

A játékos feladata, hogy minél gyorsabban és hatékonyabban gyűjtse össze az összes szükséges erőforrást és családtagot, és vigye vissza a házba. Közben figyelnie kell a kapacitásra (7 fa), az állatok mozgására, valamint a szurok előállításának folyamatára.

---

## Lehetséges továbbfejlesztések (jövőbeli ötletek)

- Időkorlát bevezetése a teljesítéshez.
- Többféle állat, különböző sebességgel és viselkedéssel.
- Dinamikus időjárás, ami befolyásolja a játékmenetet.
- Többféle faanyag különböző tulajdonságokkal.
- Több családtag "megnyerése" különféle feltételek mellett.
- Többszintű pályák nehézségi fokozatokkal.
- AI-val ellátott állatok, egyedi viselkedésekkel.
- Erőforrás-menedzsment az élelem és energia kezelésére.
- Többjátékos mód, versengő gyűjtögetés.

---

## Készítette

Molnár Márk – TLZ12Y

---

Kellemes játékot kívánok!
