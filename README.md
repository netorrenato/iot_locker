Para o codigo principal dentro da pasta projetoiot_final tem os arquivos do codigo 'Codigo_final_projeto.cpp'. Nele está implementado o controle completo do locker, incluindo a lógica em que a porta deve trancar ou destrancar com a leitura de um determinado RFID já registrado no banco de dados. Além disso, foi implementado um sistema de segurança que verifica se a porta foi deixada aberta e não há movimento, indicando que o usuário foi embora sem fechar a porta. Nessa situação, um buzzer tocará, alertando que o locker foi deixado aberto. Há, também, o controle por meio de um servidor em que o adiministrador consegue, de forma remota, dar um override e abrir ou fechar determinado locker.

O controle de registro dos usuários no banco de dados é feito no Node-red (pelo arquivo .JSON mandado também nesse repositório) e pelo MQTT. Possibilitando, assim, a interação desses dois componentes
com o locker. Além disso, existe o controle de acessos enviado por esses dois elementos, onde é registrado o usuário, horario e o estado em que o locker se encontra (aberto ou fechado). 

Um dashboard no grafana consome os dados dos bancos para explicitar informações sobre a ocupação dos lockers. 

No banco de dados foram utilizadas e criados duas tabelas: a tabela **access** com o _ID do usuário_, _estado da porta_, _data e hora de seu acesso_ e _numero do locker_; e a tabela **users** com o _nome do usuario_, seu _ID_ e seu _RFID_.

Para a montagem do circuito, utilizamos o _EasyEDA_ para montar uma PCB compacta que se adequace as nossas especificações. Utilizando o _OnShape_, fizemos e imprimimos em 3D uma caixa para proteger a PCB e garantir uma maior organização na montagem do locker.
