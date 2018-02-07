//
// Transitions.h
// Useful functions for Qt Transitions
// (c) Eric Lecolinet - Telecom ParisTech - 2016
// www.telecom-paristech.fr/~elc
//

#ifndef QTRANSITION_H
#define QTRANSITION_H

#include <QWidget>
#include <QMouseEvent>
#include <QState>
#include <QStateMachine>
#include <QSignalTransition>
#include <QKeyEventTransition>
#include <QMouseEventTransition>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/// Returns the position of the mouse cursor in widget coordinates.
inline QPoint cursorPos(QWidget* w) {
  return w->mapFromGlobal(QCursor::pos());
}

/// Returns the position of the mouse cursor in widget coordinates.
inline QPoint cursorPos(QWidget& w) {
  return w.mapFromGlobal(QCursor::pos());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/// Adds a Signal transition between _from_ and _to_.
inline QSignalTransition* addTrans(QState* from, QState* to,
                                   QObject* sender, const char* signal) {
  return from->addTransition(sender, signal, to);
}

/// Adds a Signal transition between _from_ and _to_ and connects a slot to this transition.
inline QSignalTransition* addTrans(QState* from, QState* to,
                                   QObject* sender, const char* signal,
                                   QObject* receiver, const char* slot) {
  QSignalTransition* trans = from->addTransition(sender, signal, to);
  QObject::connect(trans, SIGNAL(triggered()), receiver, slot);
  return trans;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/// Signal transition with a guard.
template <typename Guard>
class GuardedSignalTransition : public QSignalTransition {
public:
  GuardedSignalTransition(Guard& guard, QObject* sender, const char* signal, QState* from)
  : QSignalTransition(sender, signal, from), guard(guard) {}
protected:
  Guard& guard;
  bool eventTest(QEvent* e) {return guard && QSignalTransition::eventTest(e);}
};

/// Adds a guarded Signal transition between _from_ and _to_.
/** The transition is be triggered when _guard_ is false.
 *  _guard_ can be a bool or any object that can be compared to a bool.
 *  _guard_ should exist as long as the transition exists (e.g. _guard_ should not be a local variable).
 */
template <typename Guard>
inline GuardedSignalTransition<Guard>* addTrans(Guard& guard, QState* from, QState* to,
                                                QObject* sender, const char* signal) {
  GuardedSignalTransition<Guard>* trans = new GuardedSignalTransition<Guard>(guard, sender, signal, from);
  trans->setTargetState(to);
  from->addTransition(trans);
  return trans;
}

/// Adds a guarded Signal transition between _from_ and _to_ and connects a slot to this transition.
/** The transition cannot be triggered when _guard_ is false.
 *  _guard_ can be a bool or any object that can be compared to a bool.
 *  _guard_ should exist as long as the transition exists (e.g. _guard_ should not be a local variable).
 */
template <typename Guard>
inline GuardedSignalTransition<Guard>* addTrans(Guard& guard, QState* from, QState* to,
                                                QObject* sender, const char* signal,
                                                QObject* receiver, const char* slot) {
  GuardedSignalTransition<Guard>* trans = addTrans(guard, from, to, sender, signal);
  QObject::connect(trans, SIGNAL(triggered()), receiver, slot);
  return trans;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/// Adds an Event transition between _from_ and _to_.
/** @note Not compatible with User Events: use addUserTrans() instead.
 */
inline QEventTransition* addEventTrans(QState* from, QState* to,
                                       QObject* sender, QEvent::Type type) {
  QEventTransition* trans = new QEventTransition(sender, type);
  trans->setTargetState(to);
  from->addTransition(trans);
  return trans;
}

/// Adds an Event transition between _from_ and _to_ and connects a slot to this transition.
/** @note Not compatible with User Events: use addUserTrans() instead.
 */
inline QEventTransition* addEventTrans(QState* from, QState* to,
                                       QObject* sender, QEvent::Type type,
                                       QObject* receiver, const char* slot) {
  QEventTransition* trans = addEventTrans(from, to, sender, type);
  QObject::connect(trans, SIGNAL(triggered()), receiver, slot);
  return trans;
}

/// Adds a transition for Key Events between _from_ and _to_.
inline QKeyEventTransition* addKeyTrans(QState* from, QState* to,
                                        QObject* sender, QEvent::Type type, int key) {
  QKeyEventTransition* trans = new QKeyEventTransition(sender, type, key);
  trans->setTargetState(to);
  from->addTransition(trans);
  return trans;
}

/// Adds a transition for Key Events between _from_ and _to_ and connects a slot to this transition.
inline QKeyEventTransition* addKeyTrans(QState* from, QState* to,
                                        QObject* sender, QEvent::Type type, int key,
                                        QObject* receiver, const char* slot) {
  QKeyEventTransition* trans = addKeyTrans(from, to, sender, type, key);
  QObject::connect(trans, SIGNAL(triggered()), receiver, slot);
  return trans;
}

/// Adds a transition for Mouse Events between _from_ and _to_.
inline QMouseEventTransition* addMouseTrans(QState* from, QState* to,
                                            QObject* sender, QEvent::Type type, Qt::MouseButton btn) {
  QMouseEventTransition* trans = new QMouseEventTransition(sender, type, btn);
  trans->setTargetState(to);
  from->addTransition(trans);
  return trans;
}

/// Add a transition for Mouse Events between _from_ and _to_ and connects a slot to this transition.
inline QMouseEventTransition* addMouseTrans(QState* from, QState* to,
                                            QObject* sender, QEvent::Type type, Qt::MouseButton btn,
                                            QObject* receiver, const char* slot) {
  QMouseEventTransition* trans = addMouseTrans(from, to, sender, type, btn);
  QObject::connect(trans, SIGNAL(triggered()), receiver, slot);
  return trans;
}

/// Transition for Mouse Events that updates the mouse position.
class MouseEventTransition : public QMouseEventTransition {
  QPoint& pos;
public:
  MouseEventTransition(QObject* sender, QEvent::Type type, Qt::MouseButton button, QPoint& pos)
  : QMouseEventTransition(sender, type, button), pos(pos) {}
  
  virtual void onTransition(QEvent *e) {
    QEvent* realEvent = static_cast<QStateMachine::WrappedEvent*>(e)->event();
    switch (realEvent->type()) {
      case QEvent::MouseMove:
      case QEvent::MouseButtonPress:
      case QEvent::MouseButtonRelease:
      case QEvent::MouseButtonDblClick:
        pos = (static_cast<QMouseEvent*>(realEvent))->pos();
      default: break;
    }
  }
};

/// Adds a transition for Mouse Events that updates the mouse position.
/** _pos_ is a QPoint which value is updated when the transition is fired.
 *  _pos_ should exist as long as the transition exists (e.g. _pos_ should not be a local variable).
 */
inline MouseEventTransition* addMouseTrans(QState* from, QState* to,
                                           QObject* sender, QEvent::Type type, Qt::MouseButton btn,
                                           QPoint& pos) {
  MouseEventTransition* trans = new MouseEventTransition(sender, type, btn, pos);
  trans->setTargetState(to);
  from->addTransition(trans);
  return trans;
}

/// Adds a transition for Mouse Events that updates the mouse position, and connects a slot to this transition.
/** _pos_ is a QPoint which value is updated when the transition is fired.
 *  _pos_ should exist as long as the transition exists (e.g. _pos_ should not be a local variable).
 */
inline MouseEventTransition* addMouseTrans(QState* from, QState* to,
                                           QObject* sender, QEvent::Type type, Qt::MouseButton btn,
                                           QObject* receiver, const char* slot,
                                           QPoint& pos) {
  MouseEventTransition* trans = addMouseTrans(from, to, sender, type, btn, pos);
  QObject::connect(trans, SIGNAL(triggered()), receiver, slot);
  return trans;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/// Transition for User Events.
class UserEventTransition : public QAbstractTransition {
public:
  /// constructor (_eventType_ must be between QEvent::User and QEvent::MaxUser).
  UserEventTransition(int eventType) : type(eventType) {}
  int eventType() const {return type;}
  void setEventType(int eventType) {type = eventType;}
protected:
  int type;
  virtual bool eventTest(QEvent* e) {return type == e->type();}
  virtual void onTransition(QEvent*) {}
};


/// Adds a transition between _from_ and _to_ that can be fired by a User Event.
/** The transition is fired by posting a QEvent with the same _eventType_:
 *      state_machine->postEvent(new QEvent(_eventType_));
 *  @note _eventType_ must be between QEvent::User and QEvent::MaxUser.
 */
inline UserEventTransition* addUserTrans(QState* from, QState* to, int eventType) {
  UserEventTransition* trans = new UserEventTransition(eventType);
  trans->setTargetState(to);
  from->addTransition(trans);
  return trans;
}

/// Adds a transition between _from_ and _to_ that can be fired by a user event and connects a slot to this transition.
/** The transition is fired by posting a QEvent with the same _eventType_:
 *      state_machine->postEvent(new QEvent(_eventType_));
 *  @note _eventType_ must be between QEvent::User and QEvent::MaxUser.
 */
inline UserEventTransition* addUserTrans(QState* from, QState* to, int eventType,
                                         QObject* receiver, const char* slot) {
  UserEventTransition* trans = addUserTrans(from, to, eventType);
  QObject::connect(trans, SIGNAL(triggered()), receiver, slot);
  return trans;
}

#endif // TRANSITION_H
