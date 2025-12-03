Para o codigo principal dentro da pasta projetoiot_final tem nos arquivos o codigo Codigo_final_projeto.cpp. Nele é feito o controle do locker contruindo a lógica em que a porta tranca ou destranca com um determinado RFID registrado no banco de dados, além disso se a porta permanecer aberta e não houver movimento um buzzer tocará. Também há o controle por meio de um servidor em que o adiministrador abre e fecha determinado locker.
O controle de que se um usuário esta registrado no banco de dados é feito no Node-red pelo arquivo .JSON mandado também nesse repositório e pelo MQTT. Fazendo assim com que haja com a interação desses dois elementos
com o locker. Também a o controle de acessos também enviado por esses dois elementos, onde é registrado o usuário, horario e o estado em que o locker se encontra. Um dashboard no grafana consome os dados dos bancos para explicitar informacoes sobre a ocupacao dos lockers. 


