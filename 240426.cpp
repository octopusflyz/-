#include <iostream>
#include<iomanip>
#include<vector>
using namespace std;
int M=0,N = 0,R=0,K=0,T=0;//最初生命元，中间城市数，arrow攻击力，忠诚度每次降低，结束时间
int initial_hp[5] = {0};
int initial_atk[5] = {0};

int nowtime = 0;//总时间按照分钟记录
int hour = 0;
int minute = 0;
bool endflag = 0;
class city;
class warrior;
class weapon;
class headquarter;

/************************/


class city {
public:
	int num;//编号
	int hp;
	int record;//记录本座城市战绩，+2表示红色连赢两次，-2，，蓝色连赢
	bool flag;//是否插棋子
	string color;//插旗了是什么颜色
	warrior* warrior_incity[2] = {NULL,NULL};//在该城市的两个武士
	warrior* warrior_record[2] = { NULL,NULL };//临时记录在该城市的两个武士
	int sum_incity;//城市里武士总数（此时）,射箭不改变，bomb可能变，war完后统一清理
	int deadbody;//本场战斗死亡人数
	city(){}
	void plant_flag() {}
	void produce_hp() {
		hp += 10;
	}
	void pick_hp();
};
city mycity[22];
class headquarter:public city {
public:
	string side;
	headquarter(string color) :side(color) {}
	int life;//生命元
	bool stop = 0;//停止生产
	bool occupied;//是否被占领
	int enemies = 0;//目前司令部有几个敌方战士，两个则被占领
	warrior* enemies_inhead[2] = { NULL,NULL };//到达头部的敌军武士
	warrior* warrior_inhead= NULL;//产生的暂时在头部的武士

	int sum_warrior = 0;
	int num_warrior[5] = {0};//dragon,ninja,iceman,lion,wolf，各战士数量
	int order[5] = {0}; //记录生产顺序,红23401，蓝30124
	vector<warrior*> warrior_ptr;//司令部所有的warrior列表,战死不会删
	int to_make;//即将被造的战士，按它自己的order，即为造前sum%5

	headquarter() {};//默认构造
		
	bool make_ornot() {
		if (initial_hp[order[to_make]] <= life)return 1;
		else return 0;
	}
	void makewarrior();//在外面定义
	void being_occupied() {
		printf("%03d", hour);
		cout << ":10 " << side << " headquarter was taken" << '\n';
	}
	void report_life(){
		printf("%03d", hour);
		cout << ":50 " << life << " elements in " << side << " headquarter" << '\n';
	}
	~headquarter() {};
};//只生成两种head
headquarter red("red"), blue("blue");
class weapon {
public:
	string name;
	int attack;//武器攻击力
	int used;//使用过的次数
	warrior* owner=NULL;
	weapon(warrior* user):owner(user) {};
	virtual void weaken() {}
};
class sword :public weapon {
public:
	sword(warrior* user);
	void weaken() {
		attack = attack * 4 / 5;
	}
};
class arrow :public weapon {
public:
	arrow(warrior* user);
};
class bomb :public weapon {
public:
	bomb(warrior* user);
};


class warrior {
public:
	string kind;//dragon=0,ninja=1,iceman=2,lion=3,wolf=4
	string side;//红蓝
	int id;//编号
	int hp;//生命值
	int attack;//攻击力
	int place;//所在城市

	int loyalty;//忠诚度,只对lion使用
	int go_step;//走的步数，只对iceman使用，为偶数hp-9/=1，attack+20
	double morale;//士气值，只对dragon使用
	bool die_inbattle;//死于战斗，便于发放奖励

	weapon* weapon_ptr[3] = {NULL,NULL,NULL};//sword, bomb,和arrow，编号分别为0,1,2  最多各一个
	headquarter* home;//指向它出生的司令部 red or blue

	warrior(int life,int total,string color):id(total),side(color){
		die_inbattle = 0;
		//life是head所剩生命元
		if (side == "red") {
			home = &red;
			place = 0;//红方降生在城市0
			mycity[0].warrior_incity[0] = this;
			red.warrior_inhead = this;
		}
		else if (side == "blue") {
			home = &blue;
			place = N + 1;
			mycity[N + 1].warrior_incity[1] = this;
			blue.warrior_inhead = this;
		}
	};
	warrior() {};
	virtual ~warrior() {
	}
	virtual void runaway() {
		printf("%03d", hour);cout << ":05 ";
		cout << side << " lion " << id << " ran away" << '\n';
	};
	virtual void cheer() {
		printf("%03d", hour);
		cout << ":40 "<<side<<" dragon " << id << " yelled in city " << place << '\n';
	};
	virtual void march(int to_city) {
		if (to_city == 0) {
			printf("%03d", hour);
			cout << ":10 ";
			cout << side << " " << kind << " " << id << " reached red headquarter" << " with ";
			cout << hp << " elements and force " << attack << '\n';
			if(red.enemies==1&&mycity[0].warrior_record[1]!=NULL)red.being_occupied();//立马输出被占领的消息
			//printf("%03d", hour);
			//cout << ":010 red headquarter was taken" << '\n';
		}
		else if (to_city == N + 1) {
			printf("%03d", hour);
			cout << ":10 ";
			cout << side << " " << kind << " " << id << " reached blue headquarter" << " with ";
			cout << hp << " elements and force " << attack << '\n';
			if (blue.enemies == 1&&mycity[N+1].warrior_record[0]!=NULL)blue.being_occupied();
			//printf("%03d", hour);
			//cout << ":10 blue headquarter was taken" << '\n';
		}
		else {
			printf("%03d", hour);
			cout << ":10 ";
			cout << side << " " << kind << " " << id << " marched to city " << to_city << " with ";
			cout << hp << " elements and force " << attack << '\n';
		}
	};
	virtual void fight(warrior* a) {
		if (weapon_ptr[0] != NULL) {//判断有无sword
			
			a->hp -= (attack + weapon_ptr[0]->attack);
			weapon_ptr[0]->weaken();
			if (weapon_ptr[0]->attack <= 0) {
				weapon_ptr[0] = NULL;
			}
		}
		else {
			
			a->hp -= attack;
		}
	}
	virtual void fightback(warrior *a){
		if (weapon_ptr[0] != NULL) {//判断有无sword
			
			a->hp -= (attack/2 + weapon_ptr[0]->attack);
			weapon_ptr[0]->weaken();
			if (weapon_ptr[0]->attack <= 0) {
				weapon_ptr[0] = NULL;
			}
		}
		else {
			
			a->hp -= attack/2;
		}
	}
	virtual void givelife(warrior* a) {
		a += hp;
	}
	virtual void report_weapon() {
		printf("%03d", hour);
		cout << ":55 " << side << " " << kind << " " << id << " has ";
		if (weapon_ptr[0] == NULL && weapon_ptr[1] == NULL && weapon_ptr[2] == NULL) {
			cout << "no weapon" << '\n';
		}
		else {//sword, bomb,和arrow，编号分别为0,1,2
			if (weapon_ptr[2] != NULL) {
				cout << "arrow(" << 3 - weapon_ptr[2]->used << ")";
				if (weapon_ptr[1] == NULL && weapon_ptr[0] == NULL)cout << '\n';
			}
			if (weapon_ptr[1] != NULL) {
				if (weapon_ptr[2] != NULL)cout << ",";
				cout << "bomb";
				if (weapon_ptr[0] == NULL)cout << '\n';
			}
			if (weapon_ptr[0] != NULL) {
				if (weapon_ptr[2] != NULL|| weapon_ptr[1] != NULL)cout << ",";
				cout << "sword(" << weapon_ptr[0]->attack << ")" << '\n';
			}
		}
	}
};
class dragon :public warrior {
public:
	dragon(int life, int total, string color):warrior(life, total,color) {
		kind = "dragon";
		hp = initial_hp[0];
		attack = initial_atk[0];
		morale = (double)life / initial_hp[0];
		int tmp_id = id % 3;
		switch (tmp_id) {//sword, bomb,和arrow，编号分别为0,1,2
		case 0:
			weapon_ptr[0] = new sword(this);
			break;
		case 1:
			weapon_ptr[1] = new bomb(this);
			break;
		case 2:
			weapon_ptr[2] = new arrow(this);
			break;
		}

	};

};
class ninja :public warrior {
public:
	ninja(int life, int total, string color) :warrior(life, total, color) {
		kind = "ninja";
		hp = initial_hp[1];
		attack = initial_atk[1];
		int tmp_id = id % 3;
		switch (tmp_id) {
		case 0:
			weapon_ptr[0] = new sword(this);weapon_ptr[1] = new bomb(this);
			break;
		case 1:
			weapon_ptr[1] = new bomb(this);weapon_ptr[2] = new arrow(this);
			break;
		case 2:
			weapon_ptr[2] = new arrow(this);weapon_ptr[0] = new sword(this);
			break;
		}
	}

};
class iceman :public warrior {
public:
	iceman(int life, int total, string color) :warrior(life, total, color) {
		kind = "iceman";
		hp = initial_hp[2];
		attack = initial_atk[2];
		go_step = 0;
		int tmp_id = id % 3;
		switch (tmp_id) {
		case 0:
			weapon_ptr[0] = new sword(this);
			break;
		case 1:
			weapon_ptr[1] = new bomb(this);
			break;
		case 2:
			weapon_ptr[2] = new arrow(this);
			break;
		}
	}
};
class lion :public warrior {
public:
	lion(int life, int total, string color) :warrior(life, total, color) {
		kind = "lion";
		hp = initial_hp[3];
		attack = initial_atk[3];
		loyalty = life;
		//没武器
	}
};
class wolf :public warrior {
public:
	wolf(int life, int total, string color) :warrior(life, total, color) {
		kind = "wolf";
		hp = initial_hp[4];
		attack = initial_atk[4];
		//没武器，可抢
	}
};



void headquarter::makewarrior() {
	life -= initial_hp[order[to_make]];
	sum_warrior++;
	warrior* temp = NULL;
	printf("%03d", hour);cout << ":00 ";
	switch (order[to_make]) {
	case 0:
		temp = new dragon(life, sum_warrior, side);
		num_warrior[0]++;
		cout << side << " dragon " << sum_warrior << " born" << '\n';
		cout << "Its morale is " << fixed << setprecision(2) << ((double)life) / initial_hp[0] << '\n';
		break;
	case 1:
		temp = new ninja(life, sum_warrior, side);
		num_warrior[1]++;
		cout << side << " ninja " << sum_warrior << " born" << '\n';
		break;
	case 2:
		temp = new iceman(life, sum_warrior, side);
		num_warrior[2]++;
		cout << side << " iceman " << sum_warrior << " born" << '\n';
		break;
	case 3:
		temp = new lion(life, sum_warrior, side);
		num_warrior[3]++;
		cout << side << " lion " << sum_warrior << " born" << '\n';
		cout << "Its loyalty is " << life << '\n';
		break;
	case 4:
		temp = new wolf(life, sum_warrior, side);
		num_warrior[4]++;
		cout << side << " wolf " << sum_warrior << " born" << '\n';
		break;
	}
	warrior_ptr.push_back(temp);
	warrior_inhead = temp;
}
void city::pick_hp(){
	if (sum_incity == 1 && minute == 30) {
		if (warrior_incity[0] != NULL) {
			red.life += hp;
			printf("%03d", hour);
			cout << ":30 red " << warrior_incity[0]->kind<<" "<<warrior_incity[0]->id  << " earned ";
			cout << hp << " elements for his headquarter" << '\n';
			hp = 0;
		}
		else if (warrior_incity[1] != NULL) {
			blue.life += hp;
			printf("%03d", hour);
			cout << ":30 blue " << warrior_incity[1]->kind << " " << warrior_incity[1]->id  << " earned ";
			cout << hp << " elements for his headquarter" << '\n';
			hp = 0;
		}
	}
	else if (sum_incity == 2 && minute == 40) {
		if (warrior_incity[0]->hp > 0) {
			red.life += hp;
			hp = 0;
		}
		else if (warrior_incity[1]->hp > 0) {
			blue.life += hp;
			hp = 0;
		}
	}
}
sword::sword(warrior* user) :weapon(user) {
	attack = user->attack/ 5;
}
arrow::arrow(warrior* user) :weapon(user) {
	attack = R;
}
bomb::bomb(warrior* user) :weapon(user) {
}

void warrior_die(warrior* tmp) {
	if (tmp->hp <= 0) {
		mycity[tmp->place].sum_incity--;//无需？
		if (tmp->place == 0) {//红方只能死红战士
			red.warrior_inhead = NULL;
		}
		else if (tmp->place == N + 1) {
			blue.warrior_inhead = NULL;
		}
	}
	tmp = NULL;
}
void lion_escape() {
	//分别找红方和蓝方的所有武士，看有没有可能逃跑的lion且不在head的
	for (int i = 1;i <= N;i++) {
		for (int j = 0;j < 2;j++) {
			if (mycity[i].warrior_incity[j] != NULL && mycity[i].warrior_incity[j]->kind == "lion") {
				warrior* tmp = mycity[i].warrior_incity[j];
				if ((tmp)->loyalty > 0)continue;
				else {
					(tmp)->runaway();
					mycity[i].warrior_incity[j] = NULL;
					mycity[i].sum_incity--;
				}
			}
		}
	}
}
void warrior_march() {
	//考虑目的地0
	if (mycity[1].warrior_incity[1] != NULL) {
		mycity[1].warrior_incity[1]->place = 0;
		mycity[0].warrior_record[1] = mycity[1].warrior_incity[1];
		mycity[1].warrior_incity[1]->go_step++;
		
		if (mycity[1].warrior_incity[1]->kind == "iceman") {//对于iceman,输出的生命值和攻击力应该是变化后的数值
			if (mycity[1].warrior_incity[1]->go_step % 2 == 0) {
				if (mycity[1].warrior_incity[1]->hp <= 9)mycity[1].warrior_incity[1]->hp = 1;
				else {
					mycity[1].warrior_incity[1]->hp -= 9;
				}
				mycity[1].warrior_incity[1]->attack += 20;
			}
		}

		mycity[1].warrior_incity[1]->march(0);
	}
	for (int i = 0;i < N;i++) {//考虑目的地1-N
		if (mycity[i].warrior_incity[0] != NULL) {
			mycity[i].warrior_incity[0]->place = i+1;
			//先考虑红方武士移动
			mycity[i + 1].warrior_record[0] = mycity[i].warrior_incity[0];
			mycity[i].warrior_incity[0]->go_step++;
			if (mycity[i].warrior_incity[0]->kind == "iceman") {
				if (mycity[i].warrior_incity[0]->go_step % 2 == 0) {
					if (mycity[i].warrior_incity[0]->hp <= 9)mycity[i].warrior_incity[0]->hp = 1;
					else {
						mycity[i].warrior_incity[0]->hp -= 9;
					}
					mycity[i].warrior_incity[0]->attack += 20;
				}
			}
			mycity[i].warrior_incity[0]->march(i + 1);
		}
		if (mycity[i + 2].warrior_incity[1] != NULL) {
			mycity[i+2].warrior_incity[1]->place = i+1;
			//再考虑蓝方移动
			mycity[i+1].warrior_record[1] = mycity[i + 2].warrior_incity[1];
			mycity[i + 2].warrior_incity[1]->go_step++;
			if (mycity[i + 2].warrior_incity[1]->kind == "iceman") {
				if (mycity[i + 2].warrior_incity[1]->go_step % 2 == 0) {
					if (mycity[i + 2].warrior_incity[1]->hp <= 9)mycity[i + 2].warrior_incity[1]->hp = 1;
					else {
						mycity[i + 2].warrior_incity[1]->hp -= 9;
					}
					mycity[i + 2].warrior_incity[1]->attack += 20;
				}
			}
			mycity[i + 2].warrior_incity[1]->march(i + 1);
		}
	}
	//考虑目的地N+1
	if (mycity[N].warrior_incity[0] != NULL) {
		mycity[N].warrior_incity[0]->place = N+1;
		//先考虑红方武士移动
		mycity[N + 1].warrior_record[0] = mycity[N].warrior_incity[0];
		mycity[N].warrior_incity[0]->go_step++;
		if (mycity[N].warrior_incity[0]->kind == "iceman") {
			if (mycity[N].warrior_incity[0]->go_step % 2 == 0) {
				if (mycity[N].warrior_incity[0]->hp <= 9)mycity[N].warrior_incity[0]->hp = 1;
				else {
					mycity[N].warrior_incity[0]->hp -= 9;
				}
				mycity[N].warrior_incity[0]->attack += 20;
			}
		}
		mycity[N].warrior_incity[0]->march(N + 1);
	}
}
void map_change() {
	//和司令部被占领同时发生的事件，全都要输出
	for (int i = 0;i <= N+1 ;i++) {
		mycity[i].sum_incity = 0;
		for (int j = 0;j < 2;j++) {
			mycity[i].warrior_incity[j] = mycity[i].warrior_record[j];
			if (mycity[i].warrior_incity[j] != NULL)mycity[i].sum_incity++;
			mycity[i].warrior_record[j] = NULL;
		}
	}
	red.warrior_inhead = NULL;blue.warrior_inhead = NULL;
	//判断两方的head
	for (int j = 0;j < 2;j++) {//红方的
		if (mycity[0].warrior_incity[j] != NULL) {
			red.enemies++;
		}
		if (red.enemies == 2) {
			red.occupied = 1;endflag = 1;
			//red.being_occupied();
			break;
	    }
		else {
			for (int k = 0;k < 2;k++) {
				if (red.enemies_inhead[k] == NULL&& mycity[0].warrior_incity[j]!=NULL) {
					red.enemies_inhead[k] = mycity[0].warrior_incity[j];break;
				}
			}
		}
	}
	for (int j = 0;j < 2;j++) {//蓝方的
		if (mycity[N+1].warrior_incity[j] != NULL)blue.enemies++;
		if (blue.enemies == 2) {
			blue.occupied = 1;endflag = 1;
			//blue.being_occupied();
			break;
		}
		else {
			for (int k = 0;k < 2;k++) {
				if (blue.enemies_inhead[k] == NULL&& mycity[N + 1].warrior_incity[j]!=NULL) {
					blue.enemies_inhead[k] = mycity[N + 1].warrior_incity[j];break;
				}
			}
		}
	}

}
void warrior_use_arrows() {//从西向东
	//考虑红head的
	//sword, bomb, 和arrow，编号分别为0, 1, 2
	if (red.warrior_inhead != NULL && red.warrior_inhead->weapon_ptr[2] != NULL && red.warrior_inhead->weapon_ptr[2]->used < 3) {
		if (mycity[1].warrior_incity[1] != NULL) {
			mycity[1].warrior_incity[1]->hp -= R;
			red.warrior_inhead->weapon_ptr[2]->used++;
			if (red.warrior_inhead->weapon_ptr[2]->used == 3) {
				red.warrior_inhead->weapon_ptr[2] = NULL;
			}//判断arrow用了三次就没了
			if (mycity[1].warrior_incity[1]->hp > 0) {
				printf("%03d", hour);
				cout << ":35 red " << mycity[0].warrior_incity[0]->kind << " ";
				cout << mycity[0].warrior_incity[0]->id << " shot" << '\n';
			}
			else if (mycity[1].warrior_incity[1]->hp <= 0) {
				mycity[1].deadbody++;
				//mycity[1].sum_incity--;
				printf("%03d", hour);
				cout << ":35 red " << mycity[0].warrior_incity[0]->kind << " ";
				cout << mycity[0].warrior_incity[0]->id << " shot and killed blue ";
				cout << mycity[1].warrior_incity[1]->kind << " " << mycity[1].warrior_incity[1]->id << '\n';
			}
		}
	}
	//考虑红1的
		if (mycity[1].warrior_incity[0] != NULL && mycity[1].warrior_incity[0]->weapon_ptr[2] != NULL && mycity[1].warrior_incity[0]->weapon_ptr[2]->used < 3) {
			if (mycity[2].warrior_incity[1] != NULL) {
				mycity[2].warrior_incity[1]->hp -= R;
				mycity[1].warrior_incity[0]->weapon_ptr[2]->used++;
				if (mycity[1].warrior_incity[0]->weapon_ptr[2]->used == 3) {
					mycity[1].warrior_incity[0]->weapon_ptr[2] = NULL;
				}//判断arrow用了三次就没了
				if (mycity[2].warrior_incity[1]->hp > 0) {
					printf("%03d", hour);
					cout << ":35 red " << mycity[1].warrior_incity[0]->kind << " ";
					cout << mycity[1].warrior_incity[0]->id << " shot" << '\n';
				}
				else if (mycity[2].warrior_incity[1]->hp <= 0) {
					mycity[2].deadbody++;//mycity[2].sum_incity--;
					printf("%03d", hour);
					cout << ":35 red " << mycity[1].warrior_incity[0]->kind << " ";
					cout << mycity[1].warrior_incity[0]->id << " shot and killed blue ";
					cout << mycity[2].warrior_incity[1]->kind << " " << mycity[2].warrior_incity[1]->id << '\n';
				}
			}
		}
	//考虑蓝1的（不能攻击对方司令部里的敌人）
	//考虑中间城市的
	for (int i = 2;i <= N-1;i++) {//自西向东，先红后蓝
		if (mycity[i].warrior_incity[0] != NULL && mycity[i].warrior_incity[0]->weapon_ptr[2] != NULL && mycity[i].warrior_incity[0]->weapon_ptr[2]->used < 3) {
			if (mycity[i + 1].warrior_incity[1] != NULL) {
				mycity[i + 1].warrior_incity[1]->hp -= R;
				mycity[i].warrior_incity[0]->weapon_ptr[2]->used++;
				if (mycity[i].warrior_incity[0]->weapon_ptr[2]->used == 3) {
					mycity[i].warrior_incity[0]->weapon_ptr[2] = NULL;
				}//判断arrow用了三次就没了
				if (mycity[i + 1].warrior_incity[1]->hp > 0) {
					printf("%03d", hour);
					cout << ":35 red " << mycity[i].warrior_incity[0]->kind << " ";
					cout << mycity[i].warrior_incity[0]->id << " shot" << '\n';
				}
				else if (mycity[i + 1].warrior_incity[1]->hp <= 0) {
					mycity[i+1].deadbody++;//mycity[i+1].sum_incity--;
					printf("%03d", hour);
					cout << ":35 red " << mycity[i].warrior_incity[0]->kind << " ";
					cout << mycity[i].warrior_incity[0]->id << " shot and killed blue ";
					cout<< mycity[i+1].warrior_incity[1]->kind << " "<< mycity[i+1].warrior_incity[1]->id << '\n';
				}
			}
		}
		if (mycity[i].warrior_incity[1] != NULL && mycity[i].warrior_incity[1]->weapon_ptr[2] != NULL && mycity[i].warrior_incity[1]->weapon_ptr[2]->used < 3) {
			if (mycity[i - 1].warrior_incity[0] != NULL) {
				mycity[i - 1].warrior_incity[0]->hp -= R;
				mycity[i].warrior_incity[1]->weapon_ptr[2]->used++;
				if (mycity[i].warrior_incity[1]->weapon_ptr[2]->used == 3) {
					mycity[i].warrior_incity[1]->weapon_ptr[2] = NULL;
				}//判断arrow用了三次就没了
				if (mycity[i - 1].warrior_incity[0]->hp > 0) {
					printf("%03d", hour);
					cout << ":35 blue " << mycity[i].warrior_incity[1]->kind << " ";
					cout << mycity[i].warrior_incity[1]->id << " shot" << '\n';
				}
				else if (mycity[i - 1].warrior_incity[0]->hp <= 0) {
					mycity[i-1].deadbody++;//mycity[i-1].sum_incity--;
					printf("%03d", hour);
					cout << ":35 blue " << mycity[i].warrior_incity[1]->kind << " ";
					cout << mycity[i].warrior_incity[1]->id << " shot and killed red ";
					cout << mycity[i - 1].warrior_incity[0]->kind << " " << mycity[i - 1].warrior_incity[0]->id << '\n';
				}
			}
		}
	}
	//考虑红N的（不能攻击对方司令部里的敌人）
	//考虑蓝N的
	if (mycity[N].warrior_incity[1] != NULL && mycity[N].warrior_incity[1]->weapon_ptr[2] != NULL && mycity[N].warrior_incity[1]->weapon_ptr[2]->used < 3) {
		if (mycity[N- 1].warrior_incity[0] != NULL) {
			mycity[N- 1].warrior_incity[0]->hp -= R;
			mycity[N].warrior_incity[1]->weapon_ptr[2]->used++;
			if (mycity[N].warrior_incity[1]->weapon_ptr[2]->used == 3) {
				mycity[N].warrior_incity[1]->weapon_ptr[2] = NULL;
			}//判断arrow用了三次就没了
			if (mycity[N - 1].warrior_incity[0]->hp > 0) {
				printf("%03d", hour);
				cout << ":35 blue " << mycity[N].warrior_incity[1]->kind << " ";
				cout << mycity[N].warrior_incity[1]->id << " shot" << '\n';
			}
			else if (mycity[N - 1].warrior_incity[0]->hp <= 0) {
				mycity[N-1].deadbody++;//mycity[N-1].sum_incity--;
				printf("%03d", hour);
				cout << ":35 blue " << mycity[N].warrior_incity[1]->kind << " ";
				cout << mycity[N].warrior_incity[1]->id << " shot and killed red ";
				cout << mycity[N - 1].warrior_incity[0]->kind << " " << mycity[N - 1].warrior_incity[0]->id << '\n';
			}
		}
	}
	//考虑蓝head的
	if (blue.warrior_inhead != NULL && blue.warrior_inhead->weapon_ptr[2] != NULL && blue.warrior_inhead->weapon_ptr[2]->used < 3) {
		if (mycity[N ].warrior_incity[0] != NULL) {
			mycity[N ].warrior_incity[0]->hp -= R;
			blue.warrior_inhead->weapon_ptr[2]->used++;
			if (blue.warrior_inhead->weapon_ptr[2]->used == 3) {
				blue.warrior_inhead->weapon_ptr[2] = NULL;
			}//判断arrow用了三次就没了
			if (mycity[N].warrior_incity[0]->hp > 0) {
				printf("%03d", hour);
				cout << ":35 blue " << blue.warrior_inhead->kind << " ";
				cout << blue.warrior_inhead->id << " shot" << '\n';
			}
			else if (mycity[N].warrior_incity[0]->hp <= 0) {
				mycity[N].deadbody++;//mycity[N].sum_incity--;
				printf("%03d", hour);
				cout << ":35 blue " << blue.warrior_inhead->kind << " ";
				cout << blue.warrior_inhead->id << " shot and killed red ";
				cout << mycity[N ].warrior_incity[0]->kind << " " << mycity[N].warrior_incity[0]->id << '\n';
			}
		}
	}
}
void warrior_use_bombs() {
	//sword, bomb,和arrow，编号分别为0,1,2
	
	for (int i = 1;i <= N;i++) {
		warrior* red_tmp = mycity[i].warrior_incity[0];
		warrior* blue_tmp = mycity[i].warrior_incity[1];
		if (mycity[i].warrior_incity[0]!=NULL&&mycity[i].warrior_incity[1]!=NULL&& red_tmp->hp>0 && blue_tmp->hp > 0) {	
			//记录两方基本攻击力数据
			int red_atk = red_tmp->attack;int red_sword = 0;
			if (red_tmp->weapon_ptr[0] != NULL)
				red_sword = (red_tmp->weapon_ptr[0])->attack;
			int blue_atk = blue_tmp->attack;int blue_sword=0;
			if (blue_tmp->weapon_ptr[0] != NULL)
				blue_sword = (blue_tmp->weapon_ptr[0])->attack;
			//判断两方谁主动
			//红方主动
			if ((mycity[i].flag == 1 && mycity[i].color == "red") || (mycity[i].flag == 0 && mycity[i].num % 2 == 1)) {
				int red_total = red_atk + red_sword;
				int blue_total = blue_atk / 2 + blue_sword;
				//任一方使用了bomb，两者一起die
				if (blue_tmp->kind != "ninja") {
					if (blue_tmp->hp - red_total <= 0 && blue_tmp->weapon_ptr[1] != NULL) {//蓝方会死,且可以使用bomb
						mycity[i].warrior_incity[0]->hp = 0;mycity[i].warrior_incity[1]->hp = 0;
						mycity[i].warrior_incity[1]->weapon_ptr[1] = NULL;
						printf("%03d", hour);
						cout << ":38 blue " << mycity[i].warrior_incity[1]->kind << " ";
						cout << mycity[i].warrior_incity[1]->id << " used a bomb and killed red ";
						cout << mycity[i].warrior_incity[0]->kind << " " << mycity[i].warrior_incity[0]->id << '\n';
						warrior_die(mycity[i].warrior_incity[0]);mycity[i].warrior_incity[0] = NULL;
						warrior_die(mycity[i].warrior_incity[1]);mycity[i].warrior_incity[1] = NULL;
					}
					//蓝方没死，红方判断是否会被反击死
					else if (blue_tmp->hp - red_total>0&&red_tmp->hp - blue_total <= 0 && red_tmp->weapon_ptr[1] != NULL) {
						mycity[i].warrior_incity[0]->hp = 0;mycity[i].warrior_incity[1]->hp = 0;
						mycity[i].warrior_incity[0]->weapon_ptr[1] = NULL;
						printf("%03d", hour);
						cout << ":38 red " << mycity[i].warrior_incity[0]->kind << " ";
						cout << mycity[i].warrior_incity[0]->id << " used a bomb and killed blue ";
						cout << mycity[i].warrior_incity[1]->kind << " " << mycity[i].warrior_incity[1]->id << '\n';
						warrior_die(mycity[i].warrior_incity[0]);mycity[i].warrior_incity[0] = NULL;
						warrior_die(mycity[i].warrior_incity[1]);mycity[i].warrior_incity[1] = NULL;
					}
				}
				else {//ninja不会反击
					if (blue_tmp->hp - red_total <= 0 && blue_tmp->weapon_ptr[1] != NULL) {//蓝方会死,且可以使用bomb
						mycity[i].warrior_incity[0]->hp = 0;mycity[i].warrior_incity[1]->hp = 0;
						mycity[i].warrior_incity[1]->weapon_ptr[1] = NULL;
						printf("%03d", hour);
						cout << ":38 blue " << mycity[i].warrior_incity[1]->kind << " ";
						cout << mycity[i].warrior_incity[1]->id << " used a bomb and killed red ";
						cout << mycity[i].warrior_incity[0]->kind << " " << mycity[i].warrior_incity[0]->id << '\n';
						warrior_die(mycity[i].warrior_incity[0]);mycity[i].warrior_incity[0] = NULL;
						warrior_die(mycity[i].warrior_incity[1]);mycity[i].warrior_incity[1] = NULL;
					}
				}
			}
			//蓝方主动
			if ((mycity[i].flag == 1 && mycity[i].color == "blue") || (mycity[i].flag == 0 && mycity[i].num % 2 == 0)) {
				int red_total = red_atk/2 + red_sword;
				int blue_total = blue_atk+ blue_sword;
				//任一方使用了bomb，两者一起die
				if (red_tmp->kind != "ninja") {
					//if (red_tmp != NULL && red_tmp->weapon_ptr[1] != NULL)cout << "@" << '\n';
					if (red_tmp->hp - blue_total <= 0 && red_tmp->weapon_ptr[1] !=NULL) {//红方会死,且可以使用bomb
						
						mycity[i].warrior_incity[0]->hp = 0;mycity[i].warrior_incity[1]->hp = 0;
						mycity[i].warrior_incity[0]->weapon_ptr[1] = NULL;
						printf("%03d", hour);
						cout << ":38 red " << mycity[i].warrior_incity[0]->kind << " ";
						cout << mycity[i].warrior_incity[0]->id << " used a bomb and killed blue ";
						cout << mycity[i].warrior_incity[1]->kind << " " << mycity[i].warrior_incity[1]->id << '\n';
						warrior_die(mycity[i].warrior_incity[0]);mycity[i].warrior_incity[0] = NULL;
						warrior_die(mycity[i].warrior_incity[1]);mycity[i].warrior_incity[1] = NULL;
					}
					//红方没死，蓝方判断是否会被反击死
					else if (red_tmp->hp - blue_total >0 && blue_tmp->hp - red_total <= 0 && blue_tmp->weapon_ptr[1] != NULL) {
						mycity[i].warrior_incity[0]->hp = 0;mycity[i].warrior_incity[1]->hp = 0;
						mycity[i].warrior_incity[1]->weapon_ptr[1] = NULL;
						printf("%03d", hour);
						cout << ":38 blue " << mycity[i].warrior_incity[1]->kind << " ";
						cout << mycity[i].warrior_incity[1]->id << " used a bomb and killed red ";
						cout << mycity[i].warrior_incity[0]->kind << " " << mycity[i].warrior_incity[0]->id << '\n';
						warrior_die(mycity[i].warrior_incity[0]);mycity[i].warrior_incity[0] = NULL;
						warrior_die(mycity[i].warrior_incity[1]);mycity[i].warrior_incity[1] = NULL;
					}
				}
				else {//ninja不会反击
					if (red_tmp->hp - blue_total <= 0 && red_tmp->weapon_ptr[1] != NULL) {//红方会死,且可以使用bomb
						mycity[i].warrior_incity[0]->hp = 0;mycity[i].warrior_incity[1]->hp = 0;
						mycity[i].warrior_incity[0]->weapon_ptr[1] = NULL;
						printf("%03d", hour);
						cout << ":38 red " << mycity[i].warrior_incity[0]->kind << " ";
						cout << mycity[i].warrior_incity[0]->id << " used a bomb and killed blue ";
						cout << mycity[i].warrior_incity[1]->kind << " " << mycity[i].warrior_incity[1]->id << '\n';
						warrior_die(mycity[i].warrior_incity[0]);mycity[i].warrior_incity[0] = NULL;
						warrior_die(mycity[i].warrior_incity[1]);mycity[i].warrior_incity[1] = NULL;
					}
				}
			}
		}
	}
}
void battle(int i, warrior* r, warrior* b) {
	int tmp_r = r->hp;
	int tmp_b = b->hp;
	//红方主动进攻
	if ((mycity[i].flag == 1 && mycity[i].color == "red") || (mycity[i].flag == 0 && mycity[i].num % 2 == 1)) {
		r->fight(b);
		printf("%03d", hour);
		cout << ":40 red " << r->kind << " "<< r->id << " attacked blue "<< b->kind << " " << b->id ;
		cout <<" in city "<<i<<" with "<<r->hp<<" elements and force "<<r->attack<<'\n';
		//反击
		if (b->hp > 0) {//蓝没死
			if (b->kind != "ninja") {
				b->fightback(r);
				printf("%03d", hour);
				cout << ":40 blue " << b->kind << " " << b->id << " fought back against red ";
				cout << r->kind << " " << r->id << " in city " << i << '\n';
				if (r->hp <= 0) {//红死了
					mycity[i].deadbody++;
					r->die_inbattle = 1;
					printf("%03d", hour);
					cout << ":40 red " << r->kind << " " << r->id << " was killed in city " << i << '\n';
				}
			}
		}
		else {//蓝死了
			mycity[i].deadbody++;
			b->die_inbattle = 1;
			printf("%03d", hour);
			cout << ":40 blue " << b->kind << " " << b->id << " was killed in city "<<i<<'\n';
		}
	}
	//蓝方主动进攻
	if ((mycity[i].flag == 1 && mycity[i].color == "blue") || (mycity[i].flag == 0 && mycity[i].num % 2 == 0)) {
		b->fight(r);
		printf("%03d", hour);
		cout << ":40 blue " << b->kind << " " << b->id << " attacked red "<< r->kind << " " << r->id ;
		cout <<" in city "<<i<<" with " << b->hp << " elements and force " << b->attack << '\n';
		//反击
		if (r->hp > 0) {//红没死
			if (r->kind != "ninja") {
				r->fightback(b);
				printf("%03d", hour);
				cout << ":40 red " << r->kind << " " << r->id << " fought back against blue ";
				cout << b->kind << " " << b->id << " in city " << i << '\n';
			}
			if (b->hp <= 0) {//蓝死了
				mycity[i].deadbody++;
				b->die_inbattle = 1;
				printf("%03d", hour);
				cout << ":40 blue " << b->kind << " " << b->id << " was killed in city " << i << '\n';
			}
		}
		else {//红死了
			mycity[i].deadbody++;
			r->die_inbattle = 1;
			printf("%03d", hour);
			cout << ":40 red " << r->kind << " " << r->id << " was killed in city " << i << '\n';
		}
	}
	//lion贡献生命
	if (r->kind == "lion" && r->hp<=0) {//lion贡献生命
		b->hp += tmp_r;
	}
	else if (b->kind == "lion" && b->hp <= 0) {//lion贡献生命
		r->hp += tmp_b;
	}
	//注意此时先不清理deadbody
}
void war() {
	for (int i = 1;i <= N;i++) {
		//每个城市发生战斗
		if (mycity[i].warrior_incity[0] != NULL && mycity[i].warrior_incity[1] != NULL) {
			if (mycity[i].warrior_incity[0]->hp > 0 && mycity[i].warrior_incity[1]->hp > 0) {
				battle(i, mycity[i].warrior_incity[0], mycity[i].warrior_incity[1]);
			}
			//dragon欢呼,增加士气
			if (mycity[i].warrior_incity[0]->hp > 0 && mycity[i].warrior_incity[0]->kind == "dragon") {
				if (mycity[i].warrior_incity[1]->hp <= 0) {
					mycity[i].warrior_incity[0]->morale += 0.2;
				}
				else {
					mycity[i].warrior_incity[0]->morale -= 0.2;
				}
				//红方先攻击，红方可能欢呼
				if ((mycity[i].flag == 1 && mycity[i].color == "red") || (mycity[i].flag == 0 && mycity[i].num % 2 == 1)) {
					if (mycity[i].warrior_incity[0]->morale > 0.8) {
						mycity[i].warrior_incity[0]->cheer();
					}
				}
			}
			if (mycity[i].warrior_incity[1]->hp > 0 && mycity[i].warrior_incity[1]->kind == "dragon") {

				if (mycity[i].warrior_incity[0]->hp <= 0) {
					mycity[i].warrior_incity[1]->morale += 0.2;
				}
				else {
					mycity[i].warrior_incity[1]->morale -= 0.2;
				}
				//蓝方先攻击，蓝方可能欢呼
				if ((mycity[i].flag == 1 && mycity[i].color == "blue") || (mycity[i].flag == 0 && mycity[i].num % 2 == 0)) {
					if (mycity[i].warrior_incity[1]->morale > 0.8) {
						mycity[i].warrior_incity[1]->cheer();
					}
				}
			}
			//lion改变忠诚度,贡献生命值（在battle中体现）
			if (mycity[i].warrior_incity[0]->hp > 0 && mycity[i].warrior_incity[0]->kind == "lion") {
				if (mycity[i].warrior_incity[1]->hp <= 0);
				else {
					mycity[i].warrior_incity[0]->loyalty -= K;
				}
			}
			if (mycity[i].warrior_incity[1]->hp > 0 && mycity[i].warrior_incity[1]->kind == "lion") {
				if (mycity[i].warrior_incity[0]->hp <= 0);
				else {
					mycity[i].warrior_incity[1]->loyalty -= K;
				}
			}
			//wolf缴获武器
			if (mycity[i].warrior_incity[0]->hp > 0 && mycity[i].warrior_incity[0]->kind == "wolf") {
				if (mycity[i].warrior_incity[1]->hp <= 0) {
					for (int j = 0;j < 3;j++) {
						if (mycity[i].warrior_incity[1]->weapon_ptr[j] != NULL && mycity[i].warrior_incity[0]->weapon_ptr[j] == NULL)
							mycity[i].warrior_incity[0]->weapon_ptr[j] = mycity[i].warrior_incity[1]->weapon_ptr[j];
					}
				};
			}
			if (mycity[i].warrior_incity[1]->hp > 0 && mycity[i].warrior_incity[1]->kind == "wolf") {
				if (mycity[i].warrior_incity[0]->hp <= 0) {
					for (int j = 0;j < 3;j++) {
						if (mycity[i].warrior_incity[0]->weapon_ptr[j] != NULL && mycity[i].warrior_incity[1]->weapon_ptr[j] == NULL)
							mycity[i].warrior_incity[1]->weapon_ptr[j] = mycity[i].warrior_incity[0]->weapon_ptr[j];
					}
				};
			}
           //先输出earn的信息，后续再实际earn
			if (mycity[i].warrior_incity[0]->hp > 0 && mycity[i].warrior_incity[1]->hp <= 0) {
				printf("%03d", hour);
				cout << ":40 red " << mycity[i].warrior_incity[0]->kind << " " << mycity[i].warrior_incity[0]->id << " earned ";
				cout << mycity[i].hp << " elements for his headquarter" << '\n';
			}
			else if (mycity[i].warrior_incity[0]->hp <= 0 && mycity[i].warrior_incity[1]->hp > 0) {
				printf("%03d", hour);
				cout << ":40 blue " << mycity[i].warrior_incity[1]->kind << " " << mycity[i].warrior_incity[1]->id << " earned ";
				cout << mycity[i].hp << " elements for his headquarter" << '\n';
			}
			//记录一下战绩,插旗子
			if (mycity[i].warrior_incity[0]->hp > 0 && mycity[i].warrior_incity[1]->hp <= 0) {
				if (mycity[i].record<0) {
					mycity[i].record = 0;//原本是对方棋子，换棋归0，再+1
				}
				mycity[i].record++;
			}
			else if (mycity[i].warrior_incity[0]->hp <= 0 && mycity[i].warrior_incity[1]->hp > 0) {
				if (mycity[i].record>0) {
					mycity[i].record = 0;//原本是对方棋子，换棋归0，再-1
				}
				mycity[i].record--;
			}
			else if(mycity[i].warrior_incity[0]->hp > 0 && mycity[i].warrior_incity[1]->hp > 0) mycity[i].record = 0;//平局就不连续了,只包括battle和arrow，不包括bomb的
			//双方都幸存(平局)

			//cout <<"city "<<i<<" "<< mycity[i].record << '\n';

			if (mycity[i].record == 2 && (mycity[i].flag == 0 || (mycity[i].flag == 1 && mycity[i].color == "blue"))) {
				//注意原本就是自己的旗帜，经历一次平局，再连胜两次也不用更换
				    mycity[i].flag = 1;
					mycity[i].color = "red";
					printf("%03d", hour);
					cout << ":40 red flag raised in city " << i << '\n';
			}
			else if (mycity[i].record == -2 && (mycity[i].flag == 0 || (mycity[i].flag == 1 && mycity[i].color == "red"))) {
				mycity[i].flag = 1;
				mycity[i].color = "blue";
				printf("%03d", hour);
				cout << ":40 blue flag raised in city " << i << '\n';
			}
		}
	}
	
	//head奖励hp
	//红色司令部奖励武士,优先奖励距离敌方近的
	for (int i = N; i >= 1; i--) {
		if (mycity[i].warrior_incity[0] != NULL && mycity[i].warrior_incity[1] != NULL)
			if (mycity[i].warrior_incity[0]->hp > 0 && mycity[i].warrior_incity[1]->hp <= 0 && red.life >= 8) {
				
					mycity[i].warrior_incity[0]->hp += 8;
					red.life -= 8;
				
			}
	}
	//蓝色司令部奖励武士
	for (int i = 1; i <= N; i++) {
		if (mycity[i].warrior_incity[0] != NULL && mycity[i].warrior_incity[1] != NULL)
			if (mycity[i].warrior_incity[1]->hp > 0 && mycity[i].warrior_incity[0]->hp <= 0 && blue.life >= 8) {
				
					mycity[i].warrior_incity[1]->hp += 8;
					blue.life -= 8;
				
			}
	}
	//武士为head earn生命元，记录战绩
	for (int i = 1;i <= N;i++) {
		if (mycity[i].warrior_incity[0]!=NULL&&mycity[i].warrior_incity[1]!=NULL) {
			if (mycity[i].warrior_incity[0]->hp > 0 && mycity[i].warrior_incity[1]->hp <= 0) {
				mycity[i].pick_hp();
		
			}
			else if (mycity[i].warrior_incity[0]->hp <= 0 && mycity[i].warrior_incity[1]->hp > 0) {
				mycity[i].pick_hp();

			}
			else {//平局情况战绩清0
		
			}
		}
	}
	
	//清除地图上的deadbody
	for (int i = 1;i <= N;i++) {//司令部的不会被攻击
		for (int j = 0;j < 2;j++) {
			if (mycity[i].warrior_incity[j] != NULL && mycity[i].warrior_incity[j]->hp <= 0) {
				//warrior_die(mycity[i].warrior_incity[j]);
				mycity[i].warrior_incity[j] = NULL;
			}
		}
	}
}

/*****************************************/
void timeline(int endtime) {
	while (nowtime <= endtime && endflag != 1) {
		
		hour = nowtime / 60;
		minute = nowtime % 60;
		switch (minute){
		case 0://武士降生
			if (red.make_ornot() == 1) {
				red.makewarrior();
				red.to_make = red.sum_warrior % 5;
			}
			if (blue.make_ornot() == 1) {
				blue.makewarrior();
				blue.to_make = blue.sum_warrior % 5;
			}
			break;
		case 5://lion逃跑
			lion_escape();
			break;
		case 10://武士前进到某个城市，判断iceman生命值变化，武士抵达敌军head，司令部被占领（endflag）
			warrior_march();
			map_change();//（若此处endflag变为1，那么结束timeline）
			break;
		case 20://城市产生生命元
			for (int i = 1;i <= N;i++) 
			mycity[i].produce_hp();
			break;
		case 30://武士白嫖生命元
			for (int i = 1;i <= N;i++) {
				mycity[i].pick_hp();
				mycity[i].deadbody = 0;//顺便归0下尸体数
			}//（若只有一个武士）
			break;
		case 35://武士放箭,视作战斗
			warrior_use_arrows();
			break;
		case 38://武士使用bomb同归于尽
			warrior_use_bombs();
			break;
		case 40://武士主动进攻，武士反击，武士战死，dragon欢呼，lion改变忠诚度，head奖励hp，武士为head earn生命元，插旗帜
			war();
			break;
		case 50://司令部报告生命元
			
			red.report_life();
			blue.report_life();
			break;
		case 55://武士报告武器情况
			for (int i = 1;i <= N;i++) {
				if (mycity[i].warrior_incity[0] != NULL) {
					mycity[i].warrior_incity[0]->report_weapon();
				}
			}
			for (int i = 0;i < 2;i++) {
				if (blue.enemies_inhead[i] != NULL) {
					blue.enemies_inhead[i]->report_weapon();
				}
			}
			for (int i = 0;i < 2;i++) {
				if (red.enemies_inhead[i] != NULL) {
					red.enemies_inhead[i]->report_weapon();
				}
			}
			for (int i = 1;i <= N ;i++) {
				if (mycity[i].warrior_incity[1] != NULL) {
					mycity[i].warrior_incity[1]->report_weapon();
				}
			}
			
			break;
		}
		nowtime++;
	}
}
void setting() {
//基础数据设置（恢复出厂设定）
//headquarter 类的
    red.side = "red";blue.side = "blue";
	red.life = M;blue.life = M;
	red.stop = 0;blue.stop = 0;
	red.occupied = 0;blue.occupied = 0;
	red.enemies = 0;blue.enemies = 0;
    red.enemies_inhead[0] = NULL;red.enemies_inhead[1] = NULL;
	blue.enemies_inhead[0] = NULL;blue.enemies_inhead[1] = NULL;
    red.sum_warrior = 0;blue.sum_warrior = 0;
	for (int i = 0;i < 5;i++) {
		red.num_warrior[i] = 0;blue.num_warrior[i] = 0;
	}
	red.order[0] = 2;red.order[1] = 3;red.order[2] = 4;red.order[3] = 1;red.order[4] = 0;
	blue.order[0] = 3; blue.order[1] = 0; blue.order[2] = 1; blue.order[3] = 2; blue.order[4] = 4;
	red.warrior_ptr.clear();blue.warrior_ptr.clear();
	red.to_make = 0;blue.to_make = 0;
//city类的
	for (int i = 0;i <= N+1 ;i++) {
		mycity[i].num = i;
		mycity[i].hp = 0;
		mycity[i].record = 0;
		mycity[i].flag = 0;
		mycity[i].warrior_incity[0] = NULL;mycity[i].warrior_incity[1] = NULL;
		mycity[i].warrior_record[0] = NULL;mycity[i].warrior_record[1] = NULL;
		mycity[i].sum_incity = 0;
		mycity[i].deadbody = 0;
	}
//全局的
	nowtime = 0;
	hour = 0;
	minute = 0;
	endflag = 0;
};
int main()
{
	int Case;
	cin >> Case;
	for (int i = 1;i <= Case;i++) {
		/********basic information input**********/
		cin >> M >> N >> R >> K >> T;
		for (int i = 0;i < 5;i++)
			cin >> initial_hp[i];
		for (int i = 0;i < 5;i++)
			cin >> initial_atk[i];
		
		setting();
		/********start our game*************/
		cout << "Case " << i << ":" << '\n';
		timeline(T);
	}
	return 0;
}
