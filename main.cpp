#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <conio.h>
#include <cmath>
#include <stdlib.h>
#include <windows.h>

using namespace sf;
using namespace std;

const int ship_count = 5;
const Time SLEEP_TIME = seconds(2);
HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
class ship {
	Texture ship_t;
public:
	int state = 0;
	int size=0;
	int hull = 0;
	int type = 0;
	int coords[2][5] = { -1 };
	Sprite ReturnSprite() {
		Sprite ship_sprite(ship_t);
		return ship_sprite;
	}
	ship() {}
	ship(int t) {
		if (t == 0) {
			ship_t.loadFromFile("SeaWarfareSet\\battleship.png");
			size = 5;
			type = 0;
		}
		if (t == 1) {
			ship_t.loadFromFile("SeaWarfareSet\\carrier.png");
			size = 5;
			type = 1;
		}
		if (t == 2) {
			ship_t.loadFromFile("SeaWarfareSet\\cruiser.png");
			size = 4;
			type = 2;
		}
		if (t == 3) {
			ship_t.loadFromFile("SeaWarfareSet\\submarine.png");
			size = 4;
			type = 3;
		}
		if (t == 4) {
			ship_t.loadFromFile("SeaWarfareSet\\destroyer.png");
			size = 3;
			type = 4;
		}
		if (t == 5) {
			ship_t.loadFromFile("SeaWarfareSet\\comm.png");
			size = 3;
			type = 5;
		}
		hull = size;
	}
};

struct Shoots {
	int x;
	int y;
	Shoots* adr;
};
Shoots* CreateList(int lenght) {
	Shoots* cur = 0, * prev = 0, * one = 0;
	for (unsigned int i = 1; i <= lenght; i++) {
		cur = new Shoots;
		cur->x = -10;
		cur->y = -10;
		if (i > 1) {
			prev->adr = cur;
		}
		else {
			one = cur;
		}
		prev = cur;
	}
	cur->adr = 0;
	return one;
}

void AddShoot(Shoots*& beg, int pos, int x, int y) {
	Shoots* next, * one, * add, * prev = 0;
	one = beg;
	for (int i = 1; i <= pos; i++) {
		next = beg->adr;
		prev = beg;
		beg = next;
	}
	add = new Shoots;
	add->adr = beg;
	add->x = x;
	add->y = y;
	if (pos > 0) {
		prev->adr = add;
		beg = one;
	}
	else {
		beg = add;
	}

}

void PrintList(Shoots*& beg) {
	Shoots* next, * one;
	one = beg;
	while (beg) {
		next = beg->adr;
		cout << beg->x << " " << beg->y << " | ";
		beg = next;
	}
	beg = one;
}

bool SearchList(Shoots*& beg, int x, int y) {
	Shoots* next, * one;
	one = beg;
	while (beg) {
		next = beg->adr;
		if (beg->x == x && beg->y == y) { beg = one;  return true; }
		beg = next;
	}
	beg = one;
	return false;
}

void toGameCoords(int x, int y, int &game_x, int &game_y) {
	game_x = x / 40;
	game_y = y / 40;
}

bool coordsCorrect(int mx, int my, int type, int rot, bool isAI) {
	int x, y;
	if (isAI) {
		x = mx; y = my;
	}
	else {
		toGameCoords(mx, my, x, y);
	}
	if (rot == 0) {
		if (type == 0 || type == 1) {
			if (x >= 0 && x <= 9 && y >= 0 && y <= 5) return true;
			else return false;
		}
		if (type == 2 || type == 3) {
			if (x >= 0 && x <= 9 && y >= 0 && y <= 6) return true;
			else return false;
		}
		if (type == 4 || type == 5) {
			if (x >= 0 && x <= 9 && y >= 0 && y <= 7) return true;
			else return false;
		}
	}
	else {
		if (type == 0 || type == 1) {
			if (x >= 4 && x <= 9 && y >= 0 && y <= 9) return true;
			else return false;
		}
		if (type == 2 || type == 3) {
			if (x >= 3 && x <= 9 && y >= 0 && y <= 9) return true;
			else return false;
		}
		if (type == 4 || type == 5) {
			if (x >= 2 && x <= 9 && y >= 0 && y <= 9) return true;
			else return false;
		}
	}
}

bool NoclipTest(int mx, int my, int size, int rot, ship ships[], bool isAI) {
	int x, y;
	if (isAI) {
		x = mx; y = my;
	}
	else {
		toGameCoords(mx, my, x, y);
	}
		for (int i = 0; i <= ship_count; i++) {
			for (int j = 0; j < ships[i].size; j++) {
				for (int k = 0; k < size; k++) {
					if (abs(x - ships[i].coords[0][j]) <= 1 && abs(y - (ships[i].coords[1][j])+k) <= 1 && rot==0) return false;
					if (abs(x - (ships[i].coords[0][j])-k) <= 1 && abs(y - ships[i].coords[1][j]) <= 1 && rot == 1) return false;
				}
			}
		}
		return true;
}

bool coordsPossible(int x, int y) {
	if (x >= 0 && x <= 9 && y >= 0 && y <= 9) return true;
	else return false;
}

bool AIshoots(int x,int y, Shoots *&enShoots, Shoots*& enHits) {
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (SearchList(enHits, x + i, y + j)) { return false; }
		}
	}
	return true;
}

bool ShootAt(ship ships[], Sprite shoot_sprites[], int& count, int& hit_count, Shoots*& lShoots, Shoots*& lHits, int x, int y, Texture &miss, Texture &hit, bool &smartMode) {
	count++;
	AddShoot(lShoots, count, x, y);
	bool en_miss = true;
	for (int i = 0; i <= ship_count; i++) {
		for (int j = 0; j < ships[i].size; j++) {
			if (ships[i].coords[0][j] == x && ships[i].coords[1][j] == y) {
				en_miss = false;
				hit_count++;
				Sprite enemy_hit(hit);
				enemy_hit.setPosition(x * 40, y * 40);
				shoot_sprites[count] = enemy_hit;
				SetConsoleTextAttribute(handle, FOREGROUND_RED);
				cout << "Противник попал" << endl;
				AddShoot(lHits, hit_count, x, y);
				smartMode = true;
				ships[i].hull--;
				if (ships[i].hull == 0) {
					ships[i].state = 3;
					SetConsoleTextAttribute(handle, FOREGROUND_BLUE);
					cout << "Ваш корабль потоплен!!" << endl;
					smartMode = false;
				}
				break;
			}
		}
	}
	if (en_miss) {
		Sprite enemy_miss(miss);
		enemy_miss.setPosition(x * 40, y * 40);
		shoot_sprites[count] = enemy_miss;
		SetConsoleTextAttribute(handle, FOREGROUND_RED);
		cout << "Противник промахнулся" << endl;
	}
	return en_miss;
}

int main()
{
	srand(time(NULL));
	fstream file("score.txt");
	string s;
	getline(file, s);
	int num = s.length() - 1;
	int record = 0;
	int pw = 0;
	while (num >= 0) {
		record += ((((int)s[num]) - 48) * pow(10, pw));
		num -= 1;
		pw += 1;
	}
	//cout << record << endl;
	//file.close();
	//ofstream out;          
	//out.open("score.txt"); 
	
	RenderWindow app(VideoMode(1000, 700), "Battleships", Style::Close | Style::Titlebar);
	setlocale(0, "");
	//HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	Texture tile_f;
	tile_f.loadFromFile("SeaWarfareSet\\tile1.png");
	tile_f.setRepeated(true);
	Sprite grid_f(tile_f, IntRect(0, 0, 400, 400));

	Texture tile_e;
	tile_e.loadFromFile("SeaWarfareSet\\tile2.png");
	tile_e.setRepeated(true);
	Sprite grid_e(tile_e, IntRect(0, 0, 400, 400));
	grid_e.setPosition(600, 0);

	SoundBuffer buffer_explode;
	buffer_explode.loadFromFile("SeaWarfareSet\\explode.wav");
	Sound explode;
	explode.setBuffer(buffer_explode);
	SoundBuffer buffer_water;
	buffer_water.loadFromFile("SeaWarfareSet\\miss.wav");
	Sound water;
	water.setBuffer(buffer_water);
	Music music;
	music.openFromFile("SeaWarfareSet\\bg_music.ogg");
	music.setLoop(true);
	

	//Players ships
	Sprite ship_sprites[6];
	ship player_ships[6];

	ship battleship(0);
	player_ships[0] = battleship;
	ship_sprites[0] = battleship.ReturnSprite();
	ship_sprites[0].setPosition(40, 450);

	ship carrier(1);
	player_ships[1] = carrier;
	ship_sprites[1] = carrier.ReturnSprite();
	ship_sprites[1].setPosition(120, 450);

	ship cruiser(2);
	player_ships[2] = cruiser;
	ship_sprites[2] = cruiser.ReturnSprite();
	ship_sprites[2].setPosition(200, 450);

	ship sub(3);
	player_ships[3] = sub;
	ship_sprites[3] = sub.ReturnSprite();
	ship_sprites[3].setPosition(280, 450);

	ship destroyer(4);
	player_ships[4] = destroyer;
	ship_sprites[4] = destroyer.ReturnSprite();
	ship_sprites[4].setPosition(360, 450);

	ship comm(5);
	player_ships[5] = comm;
	ship_sprites[5] = comm.ReturnSprite();
	ship_sprites[5].setPosition(440, 450);
	//Enemy ships
	ship en_ships[6];

	ship battleship_e(0);
	en_ships[0] = battleship_e;

	ship carrier_e(1);
	en_ships[1] = carrier_e;

	ship cruiser_e(2);
	en_ships[2] = cruiser_e;

	ship sub_e(3);
	en_ships[3] = sub_e;

	ship destroyer_e(4);
	en_ships[4] = destroyer_e;

	ship comm_e(5);
	en_ships[5] = comm_e;

	Sprite shoot_sprites_pl[110];
	Texture tile_en_miss;
	tile_en_miss.loadFromFile("SeaWarfareSet\\tile2_miss.png");
	Texture tile_en_hit;
	tile_en_hit.loadFromFile("SeaWarfareSet\\tile2_hit.png");

	Sprite shoot_sprites_en[110];
	Texture pl_hit_exp;
	pl_hit_exp.loadFromFile("SeaWarfareSet\\explosion.png");
	Texture tile_pl_miss;
	tile_pl_miss.loadFromFile("SeaWarfareSet\\tile1_miss.png");

	bool isDragged = false;
	int dragRotation = 0;
	bool gameStarted = false;
	int shoots_player = 0, shoots_en = 0, hits_en=0;
	Shoots* plShoots = CreateList(1);
	Shoots* enShoots = CreateList(1);
	Shoots* enHits = CreateList(1);
	bool smartMode = false;
	int align = 0;
	bool align_fixed = false;
	int success_shoots = 1;
	int ax, ay;

	bool shooted = false;
	bool ai_shooted = false;
	bool is_player_missed = true;
	bool is_ai_missed = true;
	cout << "Добро пожаловать в Морской бой" << endl;
	cout << "Разместите свои корабли на поле, перетаскивая их" << endl;
	cout << "Чтобы повернуть корабль нажмите ПКМ" << endl;
	cout << "Корабли не должны касаться друг друга" << endl;
	cout << "Нажмите enter когда разместите все корабли" << endl;
	cout << "Для выхода нажмите esc" << endl;
	while (app.isOpen())
	{
		
		app.clear(Color(60, 60, 60));
		Vector2i pos = Mouse::getPosition(app);
		int x = pos.x;
		int y = pos.y;


		app.draw(grid_f);
		app.draw(grid_e);

		for (int i = 0; i <= ship_count; i++) {
			if (player_ships[i].state == 1) {
				ship_sprites[i].setPosition(x, y);
			}
			app.draw(ship_sprites[i]);
		}
		for (int i = 1; i <= shoots_player; i++) {
			app.draw(shoot_sprites_pl[i]);
		}
		for (int i = 1; i <= shoots_en; i++) {
			app.draw(shoot_sprites_en[i]);
		}
		app.display();
		int gx, gy;
		toGameCoords(x, y, gx, gy);
		//std::cout << gx << " " << gy << std::endl;
		//Противник стреляет в ответ
		if (ai_shooted) {
			if (is_ai_missed) {
				water.play();
				while (water.getStatus() == sf::SoundSource::Playing) {}
			}
			else {
				explode.play();
				while (explode.getStatus() == sf::SoundSource::Playing) {}
			}
		}
		ai_shooted = false;
		if (gameStarted && shooted) {
			if (is_player_missed) {
				water.play();
				while (water.getStatus() == sf::SoundSource::Playing) {}
			}
			else {
				explode.play();
				while (explode.getStatus() == sf::SoundSource::Playing) {}
			}
			if (smartMode) {

				if (align == 0) {
					while (true) {
						align = rand() % 4 + 1;
						if (align == 1 && coordsPossible(ax + 1, ay) && SearchList(enShoots, ax + 1, ay) == false) break;
						else if (align == 2 && coordsPossible(ax - 1, ay) && SearchList(enShoots, ax - 1, ay) == false) break;
						else if (align == 3 && coordsPossible(ax, ay + 1) && SearchList(enShoots, ax, ay + 1) == false) break;
						else if (align == 4 && coordsPossible(ax, ay - 1) && SearchList(enShoots, ax, ay - 1) == false) break;
					}
				}

				if (align == 1) {
					ax++;
				}
				else if (align == 2) {
					ax--;
				}
				else if (align == 3) {
					ay++;
				}
				else if (align == 4) {
					ay--;
				}

				bool en_miss = true;
				if (align_fixed == false)
					en_miss = ShootAt(player_ships, shoot_sprites_en, shoots_en, hits_en, enShoots, enHits, ax, ay, tile_pl_miss, pl_hit_exp, smartMode);
				else {
					for (int i = 0; i <= 2; i++) {
						if (coordsPossible(ax, ay) && SearchList(enShoots, ax, ay) == false) {
							en_miss = ShootAt(player_ships, shoot_sprites_en, shoots_en, hits_en, enShoots, enHits, ax, ay, tile_pl_miss, pl_hit_exp, smartMode);
							break;
						}
						else {
							if (align == 1) {
								ax -= success_shoots - 1;
								align = 2;
							}
							else if (align == 2) {
								ax += success_shoots + 1;
								align = 1;
							}
							else if (align == 3) {
								ay -= success_shoots - 1;
								align = 4;
							}
							else if (align == 4) {
								ay += success_shoots + 1;
								align = 3;
							}
						}
					}

				}
				if (smartMode) {
					if (en_miss == false && align_fixed == false) { align_fixed = true; success_shoots++; }
					else if (en_miss && align_fixed == false) {
						if (align == 1) {
							ax--;
						}
						else if (align == 2) {
							ax++;
						}
						else if (align == 3) {
							ay--;
						}
						else if (align == 4) {
							ay++;
						}
						while (true) {
							align++;
							if (align > 4) align = 1;
							if (align == 1 && coordsPossible(ax + 1, ay) && SearchList(enShoots, ax + 1, ay) == false) break;
							else if (align == 2 && coordsPossible(ax - 1, ay) && SearchList(enShoots, ax - 1, ay) == false) break;
							else if (align == 3 && coordsPossible(ax, ay + 1) && SearchList(enShoots, ax, ay + 1) == false) break;
							else if (align == 4 && coordsPossible(ax, ay - 1) && SearchList(enShoots, ax, ay - 1) == false) break;
						}
					}
					else if (en_miss == false && align_fixed == true) { success_shoots++; }
					else if (en_miss && align_fixed == true) {
						if (align == 1) {
							ax -= success_shoots;
							align = 2;
						}
						else if (align == 2) {
							ax += success_shoots;
							align = 1;
						}
						else if (align == 3) {
							ay -= success_shoots;
							align = 4;
						}
						else if (align == 4) {
							ay += success_shoots;
							align = 3;
						}
					}
				}
				is_ai_missed = en_miss;
			}
			else {
				align_fixed = false;
				align = 0;
				success_shoots = 1;
				do {
					ax = rand() % 10 + 0;
					ay = rand() % 10 + 0;
				} while (SearchList(enShoots, ax, ay) && AIshoots(ax, ay, enShoots, enHits) == false);
				bool m = ShootAt(player_ships, shoot_sprites_en, shoots_en, hits_en, enShoots, enHits, ax, ay, tile_pl_miss, pl_hit_exp, smartMode);
				is_ai_missed = m;
			}
			int ai_win = 0;
			for (int i = 0; i <= ship_count; i++) {
				if (player_ships[i].state == 3)  {
					ai_win++;
					}
			}
			if (ai_win >= 6) {
				SetConsoleTextAttribute(handle, FOREGROUND_BLUE);
				cout << "Вы проиграли(((" << endl;
				cout << "Рекорд " << record << " выстрелов" << endl;
				cout << "Нажмите esc для выхода" << endl;
				gameStarted = false;
			}
			shooted = false;
			ai_shooted = true;
		}
		Event event;
		while (app.pollEvent(event))
		{
			//Размещение кораблей
			if (event.type == Event::Closed)
				app.close();
			if (event.type == sf::Event::MouseButtonPressed && ai_shooted == false) {
				if (event.mouseButton.button == Mouse::Left && isDragged == true) {
					for (int i = 0; i <= ship_count; i++) {
						if (player_ships[i].state == 1 && coordsCorrect(x, y, i, dragRotation, false) && NoclipTest(x, y, player_ships[i].size, dragRotation, player_ships, false)) {
							player_ships[i].state = 2;
							isDragged = false;
							toGameCoords(x, y, player_ships[i].coords[0][0], player_ships[i].coords[1][0]);
							//std::cout << player_ships[i].coords[0][0] << " " << player_ships[i].coords[1][0] << std::endl;
							for (int j = 1; j < player_ships[i].size; j++) {
								if (dragRotation == 0) {
									player_ships[i].coords[1][j] = player_ships[i].coords[1][j - 1] + 1;
									player_ships[i].coords[0][j] = player_ships[i].coords[0][j - 1];
								}
								else {
									player_ships[i].coords[0][j] = player_ships[i].coords[0][j - 1] - 1;
									player_ships[i].coords[1][j] = player_ships[i].coords[1][j - 1];
								}
								//std::cout << player_ships[i].coords[0][j] << " " << player_ships[i].coords[1][j] << std::endl;
							}
							if (dragRotation == 0) ship_sprites[i].setPosition(player_ships[i].coords[0][0] * 40, player_ships[i].coords[1][0] * 40);
							else ship_sprites[i].setPosition((player_ships[i].coords[0][0] + 1) * 40, player_ships[i].coords[1][0] * 40);
							dragRotation = 0;
						}
					}
				}
				//Подбор кораблей
				else if (event.mouseButton.button == Mouse::Left && isDragged == false && x >= 40 && x <= 80 && y >= 450 && y <= 650) {
					if (player_ships[0].state == 0) {
						player_ships[0].state = 1;
						isDragged = true;
					}
				}
				else if (event.mouseButton.button == Mouse::Left && isDragged == false && x >= 100 && x <= 180 && y >= 450 && y <= 650) {
					if (player_ships[1].state == 0) {
						player_ships[1].state = 1;
						isDragged = true;
					}
				}
				else if (event.mouseButton.button == Mouse::Left && isDragged == false && x >= 200 && x <= 250 && y >= 450 && y <= 600) {
					if (player_ships[2].state == 0) {
						player_ships[2].state = 1;
						isDragged = true;
					}
				}
				else if (event.mouseButton.button == Mouse::Left && isDragged == false && x >= 270 && x <= 320 && y >= 450 && y <= 600) {
					if (player_ships[3].state == 0) {
						player_ships[3].state = 1;
						isDragged = true;
					}
				}
				else if (event.mouseButton.button == Mouse::Left && isDragged == false && x >= 340 && x <= 400 && y >= 450 && y <= 600) {
					if (player_ships[4].state == 0) {
						player_ships[4].state = 1;
						isDragged = true;
					}
				}
				else if (event.mouseButton.button == Mouse::Left && isDragged == false && x >= 420 && x <= 480 && y >= 450 && y <= 600) {
					if (player_ships[5].state == 0) {
						player_ships[5].state = 1;
						isDragged = true;
					}
				}
				//Стреляем
				if (event.mouseButton.button == Mouse::Left && gameStarted == true) {
					if (x >= 600 && x <= 1000 && y >= 0 && y <= 400) {
						int sx, sy;
						toGameCoords(x - 600, y, sx, sy);
						if (SearchList(plShoots, sx, sy) == false) {
							shoots_player++;
							AddShoot(plShoots, shoots_player, sx, sy);
							bool miss = true;
							for (int i = 0; i <= ship_count; i++) {
								for (int j = 0; j < en_ships[i].size; j++) {
									if (en_ships[i].coords[0][j] == sx && en_ships[i].coords[1][j] == sy) {
										miss = false;
										is_player_missed = false;
										Sprite pl_hit(tile_en_hit);
										pl_hit.setPosition(600 + sx * 40, sy * 40);
										shoot_sprites_pl[shoots_player] = pl_hit;
										SetConsoleTextAttribute(handle, FOREGROUND_GREEN);
										cout << "Вы попали" << endl;
										en_ships[i].hull--;
										if (en_ships[i].hull == 0) {
											en_ships[i].state = 3;
											SetConsoleTextAttribute(handle, FOREGROUND_BLUE);
											cout << "Вы потопили корабль!!" << endl;
										}
										break;
									}
								}
							}
							if (miss) {
								is_player_missed = true;
								Sprite pl_miss(tile_en_miss);
								pl_miss.setPosition(600 + sx * 40, sy * 40);
								shoot_sprites_pl[shoots_player] = pl_miss;
								SetConsoleTextAttribute(handle, FOREGROUND_GREEN);
								cout << "Вы промахнулись" << endl;
							}
							int player_win = 0;
							for (int i = 0; i <= ship_count; i++) {
								if (en_ships[i].state == 3)  {
									player_win++;
									}
							}
							if (player_win >= 6) {
								SetConsoleTextAttribute(handle, FOREGROUND_BLUE);
								cout << "Вы победили!!!" << endl;
								cout << "Вы сделали "<<shoots_player<<" выстрелов"<<endl;
								if (record <= shoots_player) { cout << "Рекорд "<<record<<" выстрелов" << endl; }
								else {
									file << shoots_player << std::endl;
									cout << "Новый рекорд! Прошлый рекорд " << record << " выстрелов" << endl;
									
								}
								cout << "Нажмите esc для выхода" << endl;
								gameStarted = false;
							}
							//PrintList(plShoots);
							//cout << endl;
							for (int i = 1; i <= shoots_player; i++) {
								app.draw(shoot_sprites_pl[i]);
							}
							for (int i = 1; i <= shoots_en; i++) {
								app.draw(shoot_sprites_en[i]);
							}


							shooted = true;

						}

					}

				}
				//Поворот кораблей
				if (event.mouseButton.button == Mouse::Right && isDragged == true && dragRotation == 0) {
					for (int i = 0; i <= ship_count; i++) {
						if (player_ships[i].state == 1) {
							ship_sprites[i].setRotation(90);
							dragRotation = 1;
						}
					}
				}
				else if (event.mouseButton.button == Mouse::Right && isDragged == true && dragRotation == 1) {
					for (int i = 0; i <= ship_count; i++) {
						if (player_ships[i].state == 1) {
							ship_sprites[i].setRotation(0);
							dragRotation = 0;
						}
					}
				}
				else if (event.mouseButton.button == Mouse::Right && smartMode) smartMode = false;
			}
			if (event.type == Event::KeyPressed){
				if (event.key.code == Keyboard::Enter && gameStarted == false) {
					bool complete = true;
					for (int i = 0; i <= ship_count; i++) {
						if (player_ships[i].state != 2) {
							complete = false;
							break;
						}
					}
					if (complete) {
						gameStarted = true;
						cout << "Бой начинается!" << endl;
						music.play();
						for (int i = 0; i <= ship_count; i++) {
							int ex, ey, eRot;
							do {
								ex = rand() % 10 + 0;
								ey = rand() % 10 + 0;
								eRot = rand() % 2 + 0;
							} while (coordsCorrect(ex, ey, en_ships[i].type, eRot, true) == false || NoclipTest(ex, ey, en_ships[i].size, eRot, en_ships, true) == false);
							en_ships[i].state = 2;
							en_ships[i].coords[0][0] = ex; en_ships[i].coords[1][0] = ey;
							//std::cout << en_ships[i].coords[0][0] << " " << en_ships[i].coords[1][0] << std::endl;
							for (int j = 1; j < en_ships[i].size; j++) {
								if (eRot == 0) {
									en_ships[i].coords[1][j] = en_ships[i].coords[1][j - 1] + 1;
									en_ships[i].coords[0][j] = en_ships[i].coords[0][j - 1];
								}
								else {
									en_ships[i].coords[0][j] = en_ships[i].coords[0][j - 1] - 1;
									en_ships[i].coords[1][j] = en_ships[i].coords[1][j - 1];
								}
								//std::cout << en_ships[i].coords[0][j] << " " << en_ships[i].coords[1][j] << std::endl;
							}
							//cout << "end" << endl;
						}

					}
				}
				if (event.key.code == Keyboard::Escape && gameStarted == false) {
					app.close();
				}
			}
		}

	}
	file.close();
	return 0;
}